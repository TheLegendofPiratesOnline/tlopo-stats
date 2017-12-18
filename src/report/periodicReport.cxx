#include "periodicReport.h"

#include <boost/bind.hpp>

PeriodicReport::PeriodicReport(const std::string& name,
    unsigned int period, boost::asio::io_service& io_service) : Report(name, io_service),
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

void PeriodicReport::set(doid_t key, int value)
{
    m_data[key] = value;
}

void PeriodicReport::save_task(const boost::system::error_code& e)
{
    if (e == boost::asio::error::operation_aborted)
        return;

    // TODO
    start();
}
