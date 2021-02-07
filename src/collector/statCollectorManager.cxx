#include "statCollectorManager.h"

#include "collector/statCollector.h"
#include "collector/incrementalStatCollector.h"
#include "collector/highscoreCollector.h"
#include "globals.h"

StatCollectorManager::StatCollectorManager() : m_db(nullptr),
    m_io_service(nullptr)
{
}

void StatCollectorManager::init(Database* db, boost::asio::io_service& io_service)
{
    m_db = db;
    m_io_service = &io_service;

    m_db->get_collectors(io_service, m_collectors);
    m_db->get_ban_list(m_ban_list);
}

bool StatCollectorManager::add_incremental_collector(const std::string& name,
                                                     const std::string& event)
{
    if (m_collectors.find(name) != m_collectors.end())
        return false;

    auto collector = new IncrementalStatCollector(name, event, m_db, *m_io_service);
    collector->start();

    m_collectors[name] = collector;
    m_db->add_collector(collector);
    return true;
}

bool StatCollectorManager::add_periodic_collector(const std::string& name,
                                                  const std::string& event,
                                                  unsigned int period)
{
    if (m_collectors.find(name) != m_collectors.end())
        return false;

    auto collector = new StatCollector(name, event, m_db, period, *m_io_service);
    collector->start();

    m_collectors[name] = collector;
    m_db->add_collector(collector);
    return true;
}

bool StatCollectorManager::add_highscore_collector(const std::string& name,
                                                   const std::string& event,
                                                   bool reversed)
{
    if (m_collectors.find(name) != m_collectors.end())
        return false;

    auto collector = new HighscoreCollector(name, event, m_db, reversed, *m_io_service);
    collector->start();

    m_collectors[name] = collector;
    m_db->add_collector(collector);
    return true;
}

bool StatCollectorManager::remove_collector(const std::string& name)
{
    auto it = m_collectors.find(name);
    if (it == m_collectors.end())
        return false;

    auto collector = it->second;
    m_collectors.erase(it);
    m_db->remove_collector(collector);
    delete collector;
    return true;
}

void StatCollectorManager::add_to_ban_list(doid_t id)
{
    m_ban_list.insert(id);
    m_db->add_to_ban_list(id);

    Event e;
    e.type = EVENT_LEADERBOARD_BAN;
    e.value = id;
    EventManager::get_global_ptr()->send(e);
}

bool StatCollectorManager::is_banned(doid_t id)
{
    return m_ban_list.find(id) != m_ban_list.end();
}

void StatCollectorManager::write_json(json_t** result)
{
    json_t* object = json_object();
    for (auto& it : m_collectors)
    {
        json_t* item = json_object();
        it.second->write_json(item);
        json_object_set_new(object, it.first.c_str(), item);
    }

    *result = object;
}
