#include "database.h"
#include "leaderboard.h"

#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>

#include <iostream>

using namespace mongo;

static Date_t get_date_now()
{
    return Date_t(time(NULL) * 1000);
}

class MongoDatabase : public Database {
    public:
        MongoDatabase(const std::string& url)
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

        virtual ~MongoDatabase()
        {
            delete m_client;
            m_client = nullptr;
        }

        virtual void add_entry(const std::string& name,
                               const std::string& type,
                               doid_t key,
                               long value)
        {
            try
            {
                m_client->insert(m_db + ".events", BSON(
                                 "name" << name <<
                                 "date" << get_date_now() <<
                                 "type" << type <<
                                 "key" << key <<
                                 "value" << (long long)value)
                );
            }

            catch (DBException& e)
            {
                std::cerr << "failed to add entry " << name << ": "
                          << e.what() << std::endl;
            }
        }

        virtual void add_incremental_report(const std::string& collection,
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

        virtual void read_leaderboard(const std::string& collection,
                                      Leaderboard* leaderboard)
        {
            try
            {
                BSONObj obj;
                std::auto_ptr<DBClientCursor> cursor = m_client->query(m_db + "." + collection, obj);
                while (cursor->more())
                {
                    BSONObj p = cursor->next();
                    leaderboard->set(p["_id"].Int(), p["value"].Long());
                }
            }

            catch (DBException& e)
            {
                std::cerr << "failed to read " << collection << ": "
                          << e.what() << std::endl;
            }
        }

        virtual void write_leaderboard(const std::string& collection,
                                       Leaderboard* leaderboard)
        {
            Leaderboard::entries_vec_t vec;
            leaderboard->get_sorted_entries(vec);

            try
            {
                BSONObj obj;
                m_client->remove(m_db + "." + collection, obj);

                // index 0: last updated
                m_client->insert(m_db + "." + collection, BSON(
                                 "index" << 0 <<
                                 "value" << get_date_now())
                );

                int index = 1;
                for (auto& it : vec)
                {
                    m_client->insert(m_db + "." + collection, BSON(
                                     "index" << index++ <<
                                     "rank" << it.rank <<
                                     "_id" << it.key <<
                                     "value" << (long long)it.value)
                    );
                }
            }

            catch (DBException& e)
            {
                std::cerr << "failed to write to " << collection << ": "
                          << e.what() << std::endl;
            }
        }

    private:
        DBClientBase* m_client;
        std::string m_db;
};

Database* get_mongo_db(const std::string& url)
{
    return new MongoDatabase(url);
}
