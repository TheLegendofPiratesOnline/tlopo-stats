#include "incrementalStatCollector.h"

#include "report/dailyReport.h"
#include "report/monthlyReport.h"
#include "report/yearlyReport.h"

IncrementalStatCollector::IncrementalStatCollector(const std::string& name, const std::string& event,
    Database* db, boost::asio::io_service& io_service) : StatCollectorBase(name, event, db)
{
    m_reports.push_back(new DailyReport(name, db, io_service));
    m_reports.push_back(new MonthlyReport(name, db, io_service));
    m_reports.push_back(new YearlyReport(name, db, io_service));
}

IncrementalStatCollector::~IncrementalStatCollector()
{
}

void IncrementalStatCollector::callback(const Event& e)
{
    for (auto& report : m_reports)
    {
        for (auto doId : e.doIds)
        {
            ((IncrementalPeriodicReport*)report)->increment(doId, e.value);
        }
    }
}
