#include "cache.h"

CachedDoIdMap::CachedDoIdMap(const std::string& filename) : CachedMap<doid_t, doid_t>(filename)
{
    load();
}

void CachedDoIdMap::load()
{
    json_t* data = nullptr;
    read_file(&data);
    if (!data)
        return;

    const char* key;
    json_t* value;
    json_object_foreach(data, key, value)
    {
        if (!json_is_integer(value))
            continue;

        auto keyint = std::atol(key);
        if (keyint)
            m_data[keyint] = json_integer_value(value);
    }

    json_decref(data);
}

void CachedDoIdMap::save()
{
    json_t* object = json_object();
    for (auto& it : m_data)
    {
        std::stringstream ss;
        ss << it.first;
        json_object_set_new(object, ss.str().c_str(), json_integer(it.second));
    }

    write_file(object);
}
