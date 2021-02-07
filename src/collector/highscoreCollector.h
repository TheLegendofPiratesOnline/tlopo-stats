#pragma once

#include "statCollectorBase.h"

#include "report/highscoreReport.h"

#include <boost/asio.hpp>

class HighscoreCollector : public StatCollectorBase
{
    public:
        HighscoreCollector(const std::string& name, const std::string& event,
                           Database* db, bool reversed,
                           boost::asio::io_service& io_service);
        virtual ~HighscoreCollector();

        virtual void write_json(json_t* object);

    protected:
        virtual void callback(const Event& e);

        virtual std::string get_type();

    private:
        HighscoreReport* m_report;
        bool m_reversed;
};
