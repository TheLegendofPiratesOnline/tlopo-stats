#pragma once

#include "database.h"

#include <mongo/client/dbclient.h>

class MongoDatabase : public Database {
    public:
        MongoDatabase(const std::string& url);
        virtual ~MongoDatabase();

        virtual void add_periodic_report(const std::string& collection,
                                         doid_t key,
                                         long value);
        virtual void add_incremental_report(const std::string& collection,
                                            doid_t key,
                                            long value);

    private:
        mongo::DBClientBase* m_client;
        std::string m_db;
};
