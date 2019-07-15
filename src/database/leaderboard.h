#pragma once

#include "globals.h"
#include "database.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

class Leaderboard {
    public:
        typedef struct {
            doid_t key;
            long value;
            unsigned int rank;
        } entry_t;
        typedef std::vector<entry_t> entries_vec_t;

        Leaderboard();

        void add(doid_t key, long value);
        void set(doid_t key, long value);
        void get_sorted_entries(entries_vec_t& vec);

        inline void load(Database* db, const std::string& name)
        {
            db->read_leaderboard(name, this);
        }
        inline void save(Database* db, const std::string& name)
        {
            db->write_leaderboard(name, this);
        }

        void output(std::ostream& out);

    private:
        std::unordered_map<doid_t, long> m_entries;
};

void flush_leaderboard(const std::string& collection);
