#include "database.h"

#include "debug/DLog.h"
#include "collector/statCollector.h"
#include "collector/incrementalStatCollector.h"
#include "collector/highscoreCollector.h"

#include <jansson.h>

#include <redox.hpp>

using namespace redox;

Redox rdx;
DLog dlog;

class RedisDatabase : public Database {
    public:
        RedisDatabase(const std::string& addr, int port,
                      const std::string& prefix) : m_addr(addr),
                        m_port(port), m_prefix(prefix)
        {
            connect_or_abort();
        }

        virtual ~RedisDatabase()
        {
        }

        virtual void get_collectors(boost::asio::io_service& io_service, collector_map_t& map)
        {
            dlog << "get_collectors";
            auto& c = rdx.commandSync<std::unordered_set<std::string>>(
                {"LRANGE", m_prefix + ":collectors", "0", "-1"}
            );

            if (c.ok())
            {
                dlog << "get_collectors success";
                for (auto data : c.reply())
                {
                    json_error_t error;
                    json_t* value = json_loads(data.c_str(), 0, &error);
                    if (!value || !json_is_object(value))
                    {
                        std::cerr << "ignoring invalid data in collectors" << std::endl;
                        continue;
                    }

                    std::string name = json_string_value(json_object_get(value, "name"));
                    std::string event = json_string_value(json_object_get(value, "event"));
                    std::string type = json_string_value(json_object_get(value, "type"));
                    if (type == "periodic")
                    {
                        unsigned int period = json_integer_value(json_object_get(value, "period"));
                        map[name] = new StatCollector(name, event, this, period, io_service);
                    }

                    else if (type == "incremental")
                    {
                        map[name] = new IncrementalStatCollector(name, event, this, io_service);
                    }

                    else if (type == "highscore")
                    {
                        bool reversed = json_is_true(json_object_get(value, "reversed"));
                        map[name] = new HighscoreCollector(name, event, this, reversed, io_service);
                    }

                    else
                    {
                        std::cerr << "ignoring unknown collector type " << type << std::endl;
                        json_decref(value);
                        continue;
                    }

                    map[name]->start();
                    json_decref(value);
                }
            } else {
                dlog << "get_collectors failed";
            }

            c.free();
        }

        virtual void add_collector(StatCollectorBase* collector)
        {
            dlog << "add_collector";
            json_t* item = json_object();
            collector->write_json(item);

            char* data = json_dumps(item, 0);
            rdx.command<int>({"RPUSH", m_prefix + ":collectors", data});
            free(data);

            json_decref(item);
        }

        virtual void remove_collector(StatCollectorBase* collector)
        {
            dlog << "remove_collector";
            json_t* item = json_object();
            collector->write_json(item);

            char* data = json_dumps(item, 0);
            rdx.command<int>({"LREM", m_prefix + ":collectors", "0", data});
            free(data);

            json_decref(item);
        }

        virtual void get_ban_list(doid_list_t& list)
        {
            dlog << "get_ban_list";
            auto& c = rdx.commandSync<std::unordered_set<std::string>>(
                {"LRANGE", m_prefix + ":banned", "0", "-1"}
            );

            if (c.ok()) {
                dlog << "get_ban_list success";
                for (auto id : c.reply())
                    list.insert(atoi(id.c_str()));
            } else {
                dlog << "get_ban_list failed";
            }

            c.free();
        }

        virtual void add_to_ban_list(doid_t id)
        {
            dlog << "add_to_ban_list";
            rdx.command<int>({"RPUSH", m_prefix + ":banned", std::to_string(id)});

            // Remove id from all leaderboards
            auto callback = [id](Command<std::unordered_set<std::string>>& c)
            {
                dlog << "add_to_ban_list callback";
                for (auto key : c.reply())
                    rdx.command<int>({"ZREM", key, std::to_string(id)});
            };

            rdx.command<std::unordered_set<std::string>>({"KEYS", m_prefix + ":avatar:*"},
                                                         callback);
            rdx.command<std::unordered_set<std::string>>({"KEYS", m_prefix + ":guild:*"},
                                                         callback);
        }

        virtual void get_guild_map(guild_map_t& map)
        {
            dlog << "get_guild_map";
            auto& c = rdx.commandSync<std::vector<std::string>>(
                {"HGETALL", m_prefix + ":guilds"}
            );

            if (c.ok())
            {
                dlog << "get_guild_map success";
                auto vec = c.reply();
                auto it = vec.begin();
                while (it != vec.end())
                {
                    doid_t k = atoi((*it++).c_str());
                    doid_t v = atoi((*it++).c_str());
                    map[k] = v;
                }
            } else {
                dlog << "get_guild_map fail";
            }

            c.free();
        }

        virtual void add_to_guild_map(doid_t av, doid_t guild)
        {
            dlog << "add_to_guild_map";
            rdx.command<int>({"HSET", m_prefix + ":guilds",
                              std::to_string(av),
                              std::to_string(guild)});
        }

        virtual void add_entry(const std::string& name,
                               const std::string& type,
                               doid_t key,
                               long value)
        {
            dlog << "add_entry";
            json_t* item = json_object();
            json_object_set_new(item, "name", json_string(name.c_str()));
            json_object_set_new(item, "type", json_string(type.c_str()));
            json_object_set_new(item, "key", json_integer(key));
            json_object_set_new(item, "value", json_integer(value));

            char timestamp[64];
            time_t rawtime;
            time(&rawtime);
            strftime(timestamp, 64, "%d/%b/%Y:%H:%M:%S %z", localtime(&rawtime));
            json_object_set_new(item, "time", json_string(timestamp));

            char* data = json_dumps(item, 0);
            add_entry(data);
            json_decref(item);
        }

        virtual void add_incremental_report(const std::string& collection,
                                            doid_t key,
                                            long value)
        {
            std::vector<std::string> cmd = {"ZINCRBY",
                                            m_prefix + ":" + collection,
                                            std::to_string(value),
                                            std::to_string(key)};
            add_incremental_report(cmd);
        }

        virtual void load_highscore_entries(const std::string& collection,
                                            std::unordered_map<doid_t, long>& entries)
        {
            dlog << "load_highscore_entries";
            auto& c = rdx.commandSync<std::vector<std::string>>(
                {"ZRANGE", m_prefix + ":avatar:" + collection, "0", "-1", "WITHSCORES"}
            );

            if (c.ok())
            {
                dlog << "load_highscore_entries success";
                auto vec = c.reply();
                auto it = vec.begin();
                while (it != vec.end())
                {
                    doid_t k = atoi((*it++).c_str());
                    long v = atol((*it++).c_str());
                    entries[k] = v;
                }
            } else {
                dlog << "load_highscore_entries failed";
            }

            c.free();
        }

        virtual void set_highscore_entry(const std::string& collection,
                                         doid_t key,
                                         long value)
        {
            dlog << "set_highscore_entry";
            std::vector<std::string> cmd = {"ZADD",
                                            m_prefix + ":avatar:" + collection,
                                            std::to_string(value),
                                            std::to_string(key)};
            set_highscore_entry(cmd);
        }

    private:
        void add_entry(char* data)
        {
            dlog << "add_entry";
            rdx.command<int>({"RPUSH", m_prefix + ":events", data}, [this, data](Command<int>& c) {
                if (c.ok())
                {
                    free(data);
                    return;
                }

                dlog << "add_entry failed";
                attempt_reconnect_or_abort();
                add_entry(data);
            });
        }

        void add_incremental_report(const std::vector<std::string>& cmd)
        {
            dlog << "add_incremental_report";
            rdx.command<std::string>(cmd, [this, cmd](Command<std::string>& c) {
                if (!c.ok())
                {
                    dlog << "add_incremental_report failed";
                    attempt_reconnect_or_abort();
                    add_incremental_report(cmd);
                }
            });
        }

        void set_highscore_entry(const std::vector<std::string>& cmd)
        {
            dlog << "set_highscore_entry";
            rdx.command<long long int>(cmd, [this, cmd](Command<long long int>& c) {
                if (!c.ok())
                {
                    dlog << "set_highscore_entry failed";
                    attempt_reconnect_or_abort();
                    set_highscore_entry(cmd);
                }
            });
        }

        void attempt_reconnect_or_abort(int max_attempts = 3)
        {
            dlog << "attempt_reconnect_or_abort";
            for (int i = 1; i <= max_attempts; i++)
            {
                std::cerr << "attempting reconnection " << i << "/" << max_attempts << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (rdx.connect(m_addr, m_port))
                {
                    dlog << "reconnected";
                    std::cerr << "reconnected" << std::endl;
                    return;
                }
            }

            dlog << "unable to reconnect to Redis server, giving up";
            std::cerr << "unable to reconnect to Redis server, giving up" << std::endl;
            exit(1);
        }

        void connect_or_abort()
        {
            dlog << "connect_or_abort";
            if (!rdx.connect(m_addr, m_port))
            {
                dlog << "unable to connect to Redis server";
                std::cerr << "unable to connect to Redis server" << std::endl;
                exit(1);
            }
        }

        std::string m_addr;
        int m_port;
        std::string m_prefix;
};

Database* get_redis_db(const std::string& addr, int port,
                       const std::string& prefix)
{
    return new RedisDatabase(addr, port, prefix);
}
