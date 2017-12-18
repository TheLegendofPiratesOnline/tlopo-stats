#include "statCollector.h"

#include "report/periodicReport.h"

StatCollector::StatCollector(const std::string& name, const std::string& event,
    Database* db, unsigned int period, boost::asio::io_service& io_service) : StatCollectorBase(name, event, db),
    m_period(period)
{
    m_reports.push_back(new PeriodicReport(name, period, db, io_service));
}

StatCollector::~StatCollector()
{
}

void StatCollector::callback(const Event& e)
{
    for (auto& report : m_reports)
    {
        for (auto doId : e.doIds)
        {
            ((PeriodicReport*)report)->set(doId, e.value);
        }
    }
}
