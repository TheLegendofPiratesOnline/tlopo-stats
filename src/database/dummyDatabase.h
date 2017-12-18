#pragma once

#include "database.h"

class DummyDatabase : public Database {
    public:
        DummyDatabase();
        virtual ~DummyDatabase();

        virtual void add_periodic_report(const std::string& collection,
                                         doid_t key,
                                         long value);
        virtual void add_incremental_report(const std::string& collection,
                                            doid_t key,
                                            long value);
};
