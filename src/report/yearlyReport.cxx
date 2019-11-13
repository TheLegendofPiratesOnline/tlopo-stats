#include "yearlyReport.h"

YearlyReport::YearlyReport(const std::string& name,
    Database* db,
    boost::asio::io_service& io_service) : IncrementalPeriodicReport(name, db, io_service)
{
}

YearlyReport::~YearlyReport()
{
}

unsigned int YearlyReport::time_until_next_task()
{
    time_t t1, t2;
    struct tm tms;
    time(&t1);
    localtime_r(&t1, &tms);
    tms.tm_year += 1;
    tms.tm_mon = 0;
    tms.tm_mday = 1;
    tms.tm_hour = 0;
    tms.tm_min = 0;
    tms.tm_sec = 0;
    t2 = mktime(&tms);
    return (unsigned int)(t2 - t1);
}

void YearlyReport::calc_period_string()
{
    time_t rawtime;
    time(&rawtime);
    auto timeinfo = localtime(&rawtime);

    char buffer[5];
    strftime(buffer, 5, "%Y", timeinfo);
    m_period_string = std::string(buffer);
}
