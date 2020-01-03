#include "database.h"

#include <iostream>

class DummyDatabase : public Database {
    public:
        DummyDatabase()
        {
        }

        virtual ~DummyDatabase()
        {
        }

        virtual void get_collectors(boost::asio::io_service& io_service, collector_map_t& map)
        {
        }

        virtual void add_collector(StatCollectorBase* collector)
        {
        }

        virtual void remove_collector(StatCollectorBase* collector)
        {
        }

        virtual void get_ban_list(doid_list_t& list)
        {
        }

        virtual void add_to_ban_list(doid_t id)
        {
        }

        virtual void get_guild_map(guild_map_t& map)
        {
        }

        virtual void add_to_guild_map(doid_t av, doid_t guild)
        {
        }

        virtual void add_entry(const std::string& name,
                               const std::string& type,
                               doid_t key,
                               long value)
        {
            std::cout << "DummyDatabase::add_entry" << std::endl;
            std::cout << "    name: " << name << std::endl;
            std::cout << "    type: " << type << std::endl;
            std::cout << "    key: " << key << std::endl;
            std::cout << "    value: " << value << std::endl;
        }

        virtual void add_incremental_report(const std::string& collection,
                                            doid_t key,
                                            long value)
        {
            std::cout << "DummyDatabase::add_incremental_report" << std::endl;
            std::cout << "    collection: " << collection << std::endl;
            std::cout << "    key: " << key << std::endl;
            std::cout << "    value: " << value << std::endl;
        }
};

Database* get_dummy_db()
{
    return new DummyDatabase();
}
