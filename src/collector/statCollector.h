#pragma once

#include "statCollectorBase.h"

#include <boost/asio.hpp>

class StatCollector : public StatCollectorBase
{
    public:
        StatCollector(const std::string& name, const std::string& event,
                      Database* db, unsigned int period,
                      boost::asio::io_service& io_service);
        virtual ~StatCollector();

        virtual void write_json(json_t* object);

    protected:
        virtual void callback(const Event& e);

        virtual std::string get_type();

        unsigned int m_period;
};
