#pragma once

#include "stats_types.h"

#include <jansson.h>

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

template <typename _T1, typename _T2>
class CachedMap
{
    public:
        inline CachedMap(const std::string& filename) : m_filename(filename)
        {
        }

        inline bool has_key(const _T1& key)
        {
            return m_data.count(key) != 0;
        }

        inline void erase(const _T1& key)
        {
            m_data.erase(key);
            save();
        }

        inline void set(const _T1& key, const _T2& value)
        {
            m_data[key] = value;
            save();
        }

        inline doid_t get(const _T1& key, const _T2& default_value = 0)
        {
            if (!has_key(key))
                return default_value;

            return m_data[key];
        }

    protected:
        virtual void load() = 0;
        virtual void save() = 0;

        inline void read_file(json_t** result)
        {
            std::ifstream f;
            f.open(m_filename, std::ios::binary);
            if (!f.good())
                return;

            std::stringstream ss;
            ss << f.rdbuf();
            f.close();

            json_error_t error;
            json_t* data = json_loads(ss.str().c_str(), 0, &error);
            if (!data)
            {
                std::cerr << "failed to load cache file " << m_filename \
                          << ": json_loads returned NULL: error on line " \
                          << error.line << ": " <<  error.text << std::endl;
                return;
            }

            if (!json_is_object(data))
            {
                std::cerr << "failed to load cache file " << m_filename \
                          << ": data is not an object" << std::endl;
                json_decref(data);
                return;
            }

            *result = data;
        }

        inline void write_file(json_t* object)
        {
            char* dump = json_dumps(object, 0);
            std::ofstream of;
            of.open(m_filename, std::ios::binary);
            of << std::string(dump);
            of.close();
            free(dump);

            json_decref(object);
        }

        std::unordered_map<_T1, _T2> m_data;
        std::string m_filename;
};

class CachedDoIdMap : public CachedMap<doid_t, doid_t>
{
    public:
        CachedDoIdMap(const std::string& filename);

    private:
        virtual void load();
        virtual void save();
};
