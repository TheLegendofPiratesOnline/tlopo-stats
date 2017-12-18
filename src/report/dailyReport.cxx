#include "dailyReport.h"

#include <time.h>

DailyReport::DailyReport(const std::string& name,
    boost::asio::io_service& io_service) : IncrementalPeriodicReport(name, io_service)
{
}

DailyReport::~DailyReport()
{
}

unsigned int DailyReport::time_until_next_task()
{
    time_t t1, t2;
    struct tm tms;
    time(&t1);
    localtime_r(&t1, &tms);
    tms.tm_hour = 0;
    tms.tm_min = 0;
    tms.tm_sec = 0;
    t2 = mktime(&tms);
    unsigned int elapsed = (unsigned int)(t1 - t2);
    return 86400 - elapsed;
}

void DailyReport::calc_period_string()
{
    time_t rawtime;
    time(&rawtime);
    auto timeinfo = localtime(&rawtime);

    char buffer[10];
    strftime(buffer, 10, "%Y%b%d", timeinfo);
    m_period_string = std::string(buffer);
}
