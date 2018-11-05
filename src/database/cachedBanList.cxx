#include "cachedBanList.h"

CachedBanList::CachedBanList(const std::string& filename) : CachedList<doid_t>(filename)
{
    load();
}

void CachedBanList::load()
{
    json_t* data = nullptr;
    read_file(&data);
    if (!data)
        return;

    json_t* value;
    size_t _;
    json_array_foreach(data, _, value)
    {
        if (json_is_integer(value))
            m_data.push_back(json_integer_value(value));
    }

    json_decref(data);
}

void CachedBanList::save()
{
    json_t* object = json_array();
    for (auto& it : m_data)
    {
        json_array_append_new(object, json_integer(it));
    }

    write_file(object);
}
