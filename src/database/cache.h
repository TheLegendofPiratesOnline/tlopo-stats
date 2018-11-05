#pragma once

#include "globals.h"

#include <jansson.h>

#include <unordered_map>
#include <vector>
#include <algorithm>
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

        inline _T2 get(const _T1& key, const _T2& default_value = 0)
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

template <typename _T1>
class CachedList
{
    public:
        inline CachedList(const std::string& filename) : m_filename(filename)
        {
        }

        inline void remove(const _T1& key)
        {
            m_data.erase(key);
            save();
        }

        inline void add(const _T1& key)
        {
            m_data.push_back(key);
            save();
        }

        inline bool has(const _T1& key)
        {
            return std::find(m_data.begin(), m_data.end(), key) != m_data.end();
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

            if (!json_is_array(data))
            {
                std::cerr << "failed to load cache file " << m_filename \
                          << ": data is not an array" << std::endl;
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

        std::vector<_T1> m_data;
        std::string m_filename;
};
