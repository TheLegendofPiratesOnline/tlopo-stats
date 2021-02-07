#include "highscoreCollector.h"

#include "report/highscoreReport.h"

HighscoreCollector::HighscoreCollector(const std::string& name, const std::string& event,
    Database* db, bool reversed, boost::asio::io_service& io_service) : StatCollectorBase(name, event, db),
    m_reversed(reversed)
{
    m_report = new HighscoreReport(name, reversed, db, io_service);
    m_reports.push_back(m_report);
}

HighscoreCollector::~HighscoreCollector()
{
}

void HighscoreCollector::callback(const Event& e)
{
    for (auto doId : e.doIds)
    {
        m_report->set(doId, e.value);
    }
}

void HighscoreCollector::write_json(json_t* object)
{
    StatCollectorBase::write_json(object);
    json_object_set_new(object, "reversed", json_boolean(m_reversed));
}

std::string HighscoreCollector::get_type()
{
    return "highscore";
}
