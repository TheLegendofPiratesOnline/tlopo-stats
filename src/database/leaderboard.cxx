#include "leaderboard.h"

#include <algorithm>

Leaderboard::Leaderboard()
{
}

void Leaderboard::set(doid_t key, long value)
{
    m_entries[key] = value;
}

void Leaderboard::add(doid_t key, long value)
{
    m_entries[key] += value;
}

void Leaderboard::get_sorted_entries(entries_vec_t& vec)
{
    for (auto& it : m_entries)
        vec.push_back({it.first, it.second});

    std::sort(vec.begin(), vec.end(),
              [](const auto& a, const auto& b) -> bool
    {
        return a.second > b.second;
    });
}

void Leaderboard::output(std::ostream& out)
{
    Leaderboard::entries_vec_t vec;
    get_sorted_entries(vec);

    int index = 1;
    for (auto& it : vec)
    {
        out << "    " << index++ << ". " << it.first
            << ": " << (long long)it.second << std::endl;
    }
}
