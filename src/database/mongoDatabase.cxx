#include "mongoDatabase.h"

#include <mongo/bson/bson.h>

#include <iostream>

using namespace mongo;

MongoDatabase::MongoDatabase(const std::string& url)
{
    std::string error;
    ConnectionString connection_string = ConnectionString::parse(url, error);

    if (!connection_string.isValid())
    {
        std::cerr << "invalid connection string: " << error << std::endl;
        exit(1);
    }

    m_client = connection_string.connect(error);
    if (!m_client)
    {
        std::cerr << "cannot connect to mongodb: " << error << std::endl;
        exit(1);
    }

    m_db = connection_string.getDatabase();
}

MongoDatabase::~MongoDatabase()
{
    delete m_client;
    m_client = nullptr;
}

void MongoDatabase::add_periodic_report(const std::string& collection,
                                        doid_t key,
                                        long value)
{
    try
    {
        m_client->insert(m_db + "." + collection, BSON(
                        "_id" << Date_t(time(NULL)) <<
                        "key" << key <<
                        "value" << (long long)value)
        );
    }

    catch (DBException& e)
    {
        std::cerr << "failed to write to " << collection << ": "
                  << e.what() << std::endl;
    }
}

void MongoDatabase::add_incremental_report(const std::string& collection,
                                           doid_t key,
                                           long value)
{
    try
    {
        m_client->update(m_db + "." + collection,
                         BSON("_id" << key),
                         BSON("$inc" << BSON("value" << (long long)value)),
                         true
        );
    }

    catch (DBException& e)
    {
        std::cerr << "failed to write to " << collection << ": "
                  << e.what() << std::endl;
    }
}
