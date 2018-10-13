#include "leaderboard.h"

#include <algorithm>

Leaderboard::Leaderboard() : m_entries({})
{
}

void Leaderboard::set(doid_t key, long value)
{
    if (key && value)
        m_entries[key] = value;
}

void Leaderboard::add(doid_t key, long value)
{
    if (key && value)
        m_entries[key] += value;
}

void Leaderboard::get_sorted_entries(entries_vec_t& vec)
{
    for (auto& it : m_entries)
        vec.push_back({it.first, it.second, 0});

    std::sort(vec.begin(), vec.end(),
              [](const auto& a, const auto& b) -> bool
    {
        return a.value > b.value;
    });

    int index = 0;
    unsigned int rank = 1;
    long last_value = -1;
    for (auto& it : vec)
    {
        long value = it.value;
        if (value != last_value)
        {
            last_value = value;
            rank = index + 1;
        }

        vec[index++].rank = rank;
    }
}

void Leaderboard::output(std::ostream& out)
{
    Leaderboard::entries_vec_t vec;
    get_sorted_entries(vec);

    for (auto& it : vec)
    {
        out << "    " << it.rank << ". " << it.key
            << ": " << it.value << std::endl;
    }
}
