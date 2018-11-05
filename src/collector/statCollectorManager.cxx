#include "statCollectorManager.h"

#include "collector/statCollector.h"
#include "collector/incrementalStatCollector.h"
#include "globals.h"

StatCollectorManager::StatCollectorManager() : m_db(nullptr),
    m_io_service(nullptr), m_collectors(nullptr), m_ban_list(nullptr)
{
}

void StatCollectorManager::init(Database* db, boost::asio::io_service& io_service)
{
    m_db = db;
    m_io_service = &io_service;
    m_ban_list = new CachedBanList(COLLECTOR_MGR_BAN_FILENAME);
    m_collectors = new CachedStatCollectorMap(this, COLLECTOR_MGR_CACHE_FILENAME);
}

bool StatCollectorManager::add_incremental_collector(const std::string& name,
                                                     const std::string& event)
{
    if (m_collectors->has_key(name))
        return false;

    auto collector = new IncrementalStatCollector(name, event, m_db, *m_io_service);
    collector->start();
    m_collectors->set(name, collector);
    return true;
}

bool StatCollectorManager::add_periodic_collector(const std::string& name,
                                                  const std::string& event,
                                                  unsigned int period)
{
    if (m_collectors->has_key(name))
        return false;

    auto collector = new StatCollector(name, event, m_db, period, *m_io_service);
    collector->start();
    m_collectors->set(name, collector);
    return true;
}

bool StatCollectorManager::remove_collector(const std::string& name)
{
    auto collector = m_collectors->get(name, nullptr);
    if (!collector)
        return false;

    m_collectors->erase(name);
    delete collector;
    return true;
}

void StatCollectorManager::add_to_ban_list(doid_t id)
{
    m_ban_list->add(id);
}

bool StatCollectorManager::is_banned(doid_t id)
{
    return m_ban_list->has(id);
}
