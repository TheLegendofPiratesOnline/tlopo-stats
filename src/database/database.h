#pragma once

#include "globals.h"

#include <boost/asio.hpp>

#include <unordered_map>
#include <unordered_set>
#include <string>

class StatCollectorBase; // fwd decl

typedef std::unordered_map<std::string, StatCollectorBase*> collector_map_t;
typedef std::unordered_set<doid_t> doid_list_t;
typedef std::unordered_map<doid_t, doid_t> guild_map_t;

class Database {
    public:
        Database();
        virtual ~Database();

        virtual void get_collectors(boost::asio::io_service& io_service, collector_map_t& map) = 0;
        virtual void add_collector(StatCollectorBase* collector) = 0;
        virtual void remove_collector(StatCollectorBase* collector) = 0;

        virtual void get_ban_list(doid_list_t& list) = 0;
        virtual void add_to_ban_list(doid_t id) = 0;

        virtual void get_guild_map(guild_map_t& map) = 0;
        virtual void add_to_guild_map(doid_t av, doid_t guild) = 0;

        virtual void add_entry(const std::string& name,
                               const std::string& type,
                               doid_t key,
                               long value) = 0;
        virtual void add_incremental_report(const std::string& collection,
                                            doid_t key,
                                            long value) = 0;
        virtual void load_highscore_entries(const std::string& collection,
                                            std::unordered_map<doid_t, long>& entries) = 0;
        virtual void set_highscore_entry(const std::string& collection,
                                         doid_t key,
                                         long value) = 0;
};

Database* get_dummy_db();
Database* get_redis_db(const std::string& addr, int port,
                       const std::string& prefix);
