#include "incrementalStatCollector.h"

#include "avatar/avatarManager.h"

#include "report/dailyReport.h"
#include "report/monthlyReport.h"
#include "report/yearlyReport.h"
#include "report/overallReport.h"

#include <boost/algorithm/string/predicate.hpp>

IncrementalStatCollector::IncrementalStatCollector(const std::string& name, const std::string& event,
    Database* db, boost::asio::io_service& io_service) : StatCollectorBase(name, event, db)
{
    m_reports.push_back(new DailyReport("avatar." + name, db, io_service));
    m_reports.push_back(new MonthlyReport("avatar." + name, db, io_service));
    m_reports.push_back(new YearlyReport("avatar." + name, db, io_service));
    m_reports.push_back(new OverallReport("avatar." + name, db, io_service));

    m_reports.push_back(new DailyReport("guild." + name, db, io_service));
    m_reports.push_back(new MonthlyReport("guild." + name, db, io_service));
    m_reports.push_back(new YearlyReport("guild." + name, db, io_service));
    m_reports.push_back(new OverallReport("guild." + name, db, io_service));
}

IncrementalStatCollector::~IncrementalStatCollector()
{
}

void IncrementalStatCollector::callback(const Event& e)
{
    for (auto doId : e.doIds)
    {
        auto guildId = AvatarManager::get_global_ptr()->get_guild_id(doId);

        for (auto& it : m_reports)
        {
            IncrementalPeriodicReport* report = (IncrementalPeriodicReport*)it;
            if (boost::starts_with(report->get_collection_name(), "guild."))
            {
                if (guildId)
                    report->increment(guildId, e.value);
            }

            else
            {
                report->increment(doId, e.value);
            }
        }
    }

    for (auto& it : m_reports)
    {
        IncrementalPeriodicReport* report = (IncrementalPeriodicReport*)it;
        report->flush();
    }
}

std::string IncrementalStatCollector::get_type()
{
    return "incremental";
}
