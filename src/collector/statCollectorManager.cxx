#include "statCollectorManager.h"

#include "collector/statCollector.h"
#include "collector/incrementalStatCollector.h"
#include "constants.h"

StatCollectorManager::StatCollectorManager(Database* db, boost::asio::io_service& io_service) :
  m_db(db), m_io_service(io_service), m_collectors(this, COLLECTOR_MGR_CACHE_FILENAME)
{

}

void StatCollectorManager::add_incremental_collector(const std::string& name,
                                                     const std::string& event)
{
    auto collector = new IncrementalStatCollector(name, event, m_db, m_io_service);
    collector->start();
    m_collectors.set(name, collector);
}

void StatCollectorManager::add_periodic_collector(const std::string& name,
                                                  const std::string& event,
                                                  unsigned int period)
{
    auto collector = new StatCollector(name, event, m_db, period, m_io_service);
    collector->start();
    m_collectors.set(name, collector);
}
