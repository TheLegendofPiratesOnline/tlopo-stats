#pragma once

#include "database/database.h"

#include <jansson.h>

class StatCollectorManager final {
    public:
        void init(Database* db, boost::asio::io_service& io_service);

        bool add_incremental_collector(const std::string& name, const std::string& event);
        bool add_periodic_collector(const std::string& name, const std::string& event,
                                    unsigned int period);
        bool add_highscore_collector(const std::string& name, const std::string& event,
                                     bool reversed);

        bool remove_collector(const std::string& name);

        void add_to_ban_list(doid_t id);
        bool is_banned(doid_t id);

        void write_json(json_t** result);

        static StatCollectorManager* get_global_ptr()
        {
            static StatCollectorManager* mgr = new StatCollectorManager;
            return mgr;
        }

    private:
        StatCollectorManager();

        Database* m_db;
        boost::asio::io_service* m_io_service;

        collector_map_t m_collectors;
        doid_list_t m_ban_list;

    friend class CachedStatCollectorMap;
};
