#include "highscoreReport.h"

HighscoreReport::HighscoreReport(const std::string& name,
    Database* db,
    boost::asio::io_service& io_service) : Report(name, db, io_service)
{
}

HighscoreReport::~HighscoreReport()
{
}

void HighscoreReport::start()
{
    m_db->load_highscore_entries(m_name, m_entries);
}

void HighscoreReport::set(doid_t key, long value)
{
    auto it = m_entries.find(key);
    if (it != m_entries.end() && it->second >= value)
        return; // No update

    m_entries[key] = value;
    m_db->set_highscore_entry(m_name, key, value);
}
