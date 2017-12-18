#pragma once

#include <boost/asio.hpp>

#include <string>

class Report {
    public:
        Report(const std::string& name, boost::asio::io_service& io_service);
        virtual ~Report();

        virtual void start() = 0;
        virtual std::string get_collection_name();

    protected:
        std::string m_name;

        boost::asio::deadline_timer m_timer;
};
