#pragma once

#include "statCollectorBase.h"

#include <boost/asio.hpp>

class IncrementalStatCollector : public StatCollectorBase
{
    public:
        IncrementalStatCollector(const std::string& name, const std::string& event,
                      Database* db, boost::asio::io_service& io_service);
        virtual ~IncrementalStatCollector();

    protected:
        virtual void callback(const Event& e);
};
