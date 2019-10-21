#include "database.h"
#include "leaderboard.h"

#include <iostream>

class DummyDatabase : public Database {
    public:
        DummyDatabase()
        {
        }

        virtual ~DummyDatabase()
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

        virtual void read_leaderboard(const std::string& collection,
                                      Leaderboard* leaderboard)
        {
            std::cout << "DummyDatabase::read_leaderboard " << collection << std::endl;
        }

        virtual void write_leaderboard(const std::string& collection,
                                       Leaderboard* leaderboard)
        {
            std::cout << "DummyDatabase::write_leaderboard " << collection << std::endl;
            leaderboard->output(std::cout);
        }
};

Database* get_dummy_db()
{
    return new DummyDatabase();
}
