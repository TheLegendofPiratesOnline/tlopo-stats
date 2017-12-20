#pragma once

#include "event/eventListener.h"
#include "report/report.h"

#include <jansson.h>

#include <vector>

class StatCollectorBase : private EventListener
{
    public:
        StatCollectorBase(const std::string& name, const std::string& event, Database* db);
        virtual ~StatCollectorBase();

        virtual void write_json(json_t* object);
        void start();

    protected:
        virtual void callback(const Event& e) = 0;

        virtual std::string get_type() = 0;

        std::string m_name;
        std::string m_event;
        Database* m_db;
        std::vector<Report*> m_reports;
};
