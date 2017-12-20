#include "cachedStatCollectorMap.h"

#include "collector/statCollectorManager.h"
#include "collector/statCollector.h"
#include "collector/incrementalStatCollector.h"

CachedStatCollectorMap::CachedStatCollectorMap(StatCollectorManager* mgr, const std::string& filename) :
  CachedMap<std::string, StatCollectorBase*>(filename), m_mgr(mgr)
{
    load();
}

void CachedStatCollectorMap::load()
{
    json_t* data = nullptr;
    read_file(&data);
    if (!data)
        return;

    const char* key;
    json_t* value;
    json_object_foreach(data, key, value)
    {
        std::string name = json_string_value(json_object_get(value, "name"));
        std::string event = json_string_value(json_object_get(value, "event"));
        std::string type = json_string_value(json_object_get(value, "type"));
        if (type == "periodic")
        {
            unsigned int period = json_integer_value(json_object_get(value, "period"));
            m_data[name] = new StatCollector(name, event, m_mgr->m_db, period, *m_mgr->m_io_service);
            m_data[name]->start();
        }

        else if (type == "incremental")
        {
            m_data[name] = new IncrementalStatCollector(name, event, m_mgr->m_db, *m_mgr->m_io_service);
            m_data[name]->start();
        }

        else
        {
            std::cerr << "ignoring unknown type " << type << std::endl;
        }
    }

    json_decref(data);
}

void CachedStatCollectorMap::write_json(json_t** result)
{
    json_t* object = json_object();
    for (auto& it : m_data)
    {
        json_t* item = json_object();
        it.second->write_json(item);
        json_object_set_new(object, it.first.c_str(), item);
    }

    *result = object;
}

void CachedStatCollectorMap::save()
{
    json_t* object;
    write_json(&object);
    write_file(object);
}
