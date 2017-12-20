#include "statCollectorBase.h"

StatCollectorBase::StatCollectorBase(const std::string& name,
    const std::string& event, Database* db) : m_name(name),
    m_event(event), m_db(db)
{

}

StatCollectorBase::~StatCollectorBase()
{
    for (auto& report : m_reports)
        delete report;

    m_reports.clear();
    disconnect();
}

void StatCollectorBase::start()
{
    listen(m_event, [this](const Event& e)
    {
        this->callback(e);
    });

    for (auto& report : m_reports)
        report->start();
}

void StatCollectorBase::write_json(json_t* object)
{
    json_object_set_new(object, "name", json_string(m_name.c_str()));
    json_object_set_new(object, "event", json_string(m_event.c_str()));
    json_object_set_new(object, "type", json_string(get_type().c_str()));
}
