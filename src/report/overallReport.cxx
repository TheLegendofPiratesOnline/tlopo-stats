#include "overallReport.h"

OverallReport::OverallReport(const std::string& name,
    Database* db, boost::asio::io_service& io_service,
    bool leaderboard) : IncrementalPeriodicReport(name, db, io_service, leaderboard)
{
}

OverallReport::~OverallReport()
{
}

unsigned int OverallReport::time_until_next_task()
{
    return INF;
}

void OverallReport::calc_period_string()
{
    m_period_string = "overall";
}
