#include "incrementalPeriodicReport.h"

#include <boost/bind.hpp>

IncrementalPeriodicReport::IncrementalPeriodicReport(const std::string& name,
    Database* db, boost::asio::io_service& io_service,
    bool leaderboard) : Report(name, db, io_service), m_ld_timer(io_service)
{
    if (leaderboard)
        m_leaderboard = new Leaderboard;

    else
        m_leaderboard = nullptr;
}

IncrementalPeriodicReport::~IncrementalPeriodicReport()
{
}

void IncrementalPeriodicReport::start()
{
    boost::system::error_code noerror;
    rotate(noerror);
}

void IncrementalPeriodicReport::flush_leaderboard(std::string name)
{
    if (!name.size())
        name = "ld." + get_collection_name();

    m_leaderboard->save(m_db, name);
}

void IncrementalPeriodicReport::flush()
{
    for (auto& it : m_temp_cache)
    {
        m_leaderboard->add(it.first, it.second);
        m_db->add_incremental_report(get_collection_name(), it.first, it.second);
    }

    m_temp_cache.clear();
}

void IncrementalPeriodicReport::increment(doid_t key, long value)
{
    m_temp_cache[key] += value;
}

std::string IncrementalPeriodicReport::get_collection_name()
{
    return m_name + "." + m_period_string;
}

void IncrementalPeriodicReport::rotate(const boost::system::error_code& e)
{
    if (e == boost::asio::error::operation_aborted)
        return;

    std::string prev_period_string = m_period_string;
    calc_period_string();

    if (!prev_period_string.size())
    {
        // Initialize leaderboard, if any
        if (m_leaderboard)
        {
            m_leaderboard->load(m_db, get_collection_name());

            boost::system::error_code noerror;
            leaderboard_task(noerror);
        }
    }

    else if (prev_period_string != m_period_string)
    {
        // Flush previous leaderboard
        flush_leaderboard("ld." + m_name + "." + prev_period_string);

        delete m_leaderboard;
        m_leaderboard = new Leaderboard;
    }

    auto delay = std::max(time_until_next_task(), (unsigned int)1);
    if (delay == INF)
        return;

    m_timer.expires_from_now(boost::posix_time::seconds(delay));
    m_timer.async_wait(boost::bind(&IncrementalPeriodicReport::rotate,
                                   this, boost::asio::placeholders::error));
}

void IncrementalPeriodicReport::leaderboard_task(const boost::system::error_code& e)
{
    if (e == boost::asio::error::operation_aborted)
        return;

    flush_leaderboard("");
    m_ld_timer.expires_from_now(boost::posix_time::seconds(get_ld_flush_period()));
    m_ld_timer.async_wait(boost::bind(&IncrementalPeriodicReport::leaderboard_task,
                                      this, boost::asio::placeholders::error));
}
