#include "dummyDatabase.h"
#include "leaderboard.h"

#include <iostream>

DummyDatabase::DummyDatabase()
{
}

DummyDatabase::~DummyDatabase()
{
}

void DummyDatabase::add_periodic_report(const std::string& collection,
                                        doid_t key,
                                        long value)
{
    std::cout << "DummyDatabase::add_periodic_report" << std::endl;
    std::cout << "    collection: " << collection << std::endl;
    std::cout << "    key: " << key << std::endl;
    std::cout << "    value: " << value << std::endl;
}

void DummyDatabase::add_incremental_report(const std::string& collection,
                                           doid_t key,
                                           long value)
{
    std::cout << "DummyDatabase::add_incremental_report" << std::endl;
    std::cout << "    collection: " << collection << std::endl;
    std::cout << "    key: " << key << std::endl;
    std::cout << "    value: " << value << std::endl;
}

void DummyDatabase::read_leaderboard(const std::string& collection,
                                     Leaderboard* leaderboard)
{
    std::cout << "DummyDatabase::read_leaderboard " << collection << std::endl;
}

void DummyDatabase::write_leaderboard(const std::string& collection,
                                      Leaderboard* leaderboard)
{
    std::cout << "DummyDatabase::write_leaderboard " << collection << std::endl;
    leaderboard->output(std::cout);
}
