#pragma once

#include "stats_types.h"

#include <string>

class Database {
    public:
        Database();
        virtual ~Database();

        virtual void add_periodic_report(const std::string& collection,
                                         doid_t key,
                                         long value) = 0;
        virtual void add_incremental_report(const std::string& collection,
                                            doid_t key,
                                            long value) = 0;
};
