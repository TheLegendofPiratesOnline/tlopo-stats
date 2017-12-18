#include "report.h"

Report::Report(const std::string& name, Database* db,
    boost::asio::io_service& io_service) : m_name(name),
    m_db(db), m_timer(io_service)
{
}

Report::~Report()
{
    m_timer.cancel();
}

std::string Report::get_collection_name()
{
    // Return m_name by default
    return m_name;
}
