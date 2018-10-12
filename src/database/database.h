#pragma once

#include "globals.h"

#include <string>

class Leaderboard; // fwd decl

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
        virtual void read_leaderboard(const std::string& collection,
                                      Leaderboard* leaderboard) = 0;
        virtual void write_leaderboard(const std::string& collection,
                                       Leaderboard* leaderboard) = 0;
};
