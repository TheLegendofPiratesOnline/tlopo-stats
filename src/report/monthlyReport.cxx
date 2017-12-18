#include "monthlyReport.h"

MonthlyReport::MonthlyReport(const std::string& name,
    boost::asio::io_service& io_service) : IncrementalPeriodicReport(name, io_service)
{
}

MonthlyReport::~MonthlyReport()
{
}

unsigned int MonthlyReport::time_until_next_task()
{
    time_t t1, t2;
    struct tm tms;
    time(&t1);
    localtime_r(&t1, &tms);
    tms.tm_mday = 1;
    tms.tm_hour = 0;
    tms.tm_min = 0;
    tms.tm_sec = 0;

    if (tms.tm_mon == 11)
    {
        tms.tm_year += 1;
        tms.tm_mon = 0;
    }

    else
    {
        tms.tm_mon += 1;
    }

    t2 = mktime(&tms);
    return (unsigned int)(t2 - t1);
}

void MonthlyReport::calc_period_string()
{
    time_t rawtime;
    time(&rawtime);
    auto timeinfo = localtime(&rawtime);

    char buffer[8];
    strftime(buffer, 8, "%Y%b", timeinfo);
    m_period_string = std::string(buffer);
}
