#include "incrementalPeriodicReport.h"

#include <boost/bind.hpp>

IncrementalPeriodicReport::IncrementalPeriodicReport(const std::string& name,
    Database* db, boost::asio::io_service& io_service) : Report(name, db, io_service)
{
}

IncrementalPeriodicReport::~IncrementalPeriodicReport()
{
}

void IncrementalPeriodicReport::start()
{
    boost::system::error_code noerror;
    rotate(noerror);
}

void IncrementalPeriodicReport::flush()
{
    for (auto& it : m_temp_cache)
        m_db->add_incremental_report(get_collection_name(), it.first, it.second);

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

    calc_period_string();

    auto delay = std::max(time_until_next_task(), (unsigned int)1);
    m_timer.expires_from_now(boost::posix_time::seconds(delay));
    m_timer.async_wait(boost::bind(&IncrementalPeriodicReport::rotate,
                                   this, boost::asio::placeholders::error));
}
