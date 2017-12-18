#pragma once

#include "event/eventListener.h"
#include "report/report.h"

#include <vector>

class StatCollectorBase : private EventListener
{
    public:
        StatCollectorBase(const std::string& name, const std::string& event, Database* db);
        virtual ~StatCollectorBase();

        void start();

    protected:
        virtual void callback(const Event& e) = 0;

        std::string m_name;
        std::string m_event;
        Database* m_db;
        std::vector<Report*> m_reports;
};
