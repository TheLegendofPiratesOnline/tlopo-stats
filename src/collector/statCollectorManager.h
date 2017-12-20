#pragma once

#include "database/database.h"
#include "database/cachedStatCollectorMap.h"

class StatCollectorManager final {
    public:
        StatCollectorManager(Database* db, boost::asio::io_service& io_service);

        void add_incremental_collector(const std::string& name, const std::string& event);
        void add_periodic_collector(const std::string& name, const std::string& event,
                                    unsigned int period);

    private:
        Database* m_db;
        boost::asio::io_service& m_io_service;

        CachedStatCollectorMap m_collectors;

    friend class CachedStatCollectorMap;
};
