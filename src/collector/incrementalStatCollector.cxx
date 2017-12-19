#include "incrementalStatCollector.h"

#include "avatar/avatarManager.h"

#include "report/dailyReport.h"
#include "report/monthlyReport.h"
#include "report/yearlyReport.h"

#include <boost/algorithm/string/predicate.hpp>

IncrementalStatCollector::IncrementalStatCollector(const std::string& name, const std::string& event,
    Database* db, boost::asio::io_service& io_service) : StatCollectorBase(name, event, db)
{
    m_reports.push_back(new DailyReport(name, db, io_service));
    m_reports.push_back(new MonthlyReport(name, db, io_service));
    m_reports.push_back(new YearlyReport(name, db, io_service));

    m_reports.push_back(new DailyReport("guild_" + name, db, io_service));
    m_reports.push_back(new MonthlyReport("guild_" + name, db, io_service));
    m_reports.push_back(new YearlyReport("guild_" + name, db, io_service));
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
            if (boost::starts_with(report->get_collection_name(), "guild_"))
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
}
