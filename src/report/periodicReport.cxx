#include "periodicReport.h"

#include <boost/bind.hpp>

PeriodicReport::PeriodicReport(const std::string& name,
    unsigned int period, Database* db,
    boost::asio::io_service& io_service) : Report(name, db, io_service),
    m_period(period)
{
}

PeriodicReport::~PeriodicReport()
{
}

void PeriodicReport::start()
{
    m_timer.expires_from_now(boost::posix_time::minutes(m_period));
    m_timer.async_wait(boost::bind(&PeriodicReport::save_task,
                                    this, boost::asio::placeholders::error));
}

void PeriodicReport::set(doid_t key, long value)
{
    m_data[key] = value;
}

void PeriodicReport::save_task(const boost::system::error_code& e)
{
    if (e == boost::asio::error::operation_aborted)
        return;

    for (auto& it : m_data)
    {
        m_db->add_periodic_report(get_collection_name(), it.first, it.second);
    }

    start();
}
