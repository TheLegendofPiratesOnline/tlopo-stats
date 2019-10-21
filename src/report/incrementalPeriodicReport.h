#pragma once

#include "database/leaderboard.h"
#include "report.h"
#include "globals.h"

#include <unordered_map>

class IncrementalPeriodicReport : public Report {
    public:
        IncrementalPeriodicReport(const std::string& name, Database* db,
                                  boost::asio::io_service& io_service);
        virtual ~IncrementalPeriodicReport();

        virtual void start();

        virtual std::string get_collection_name();
        void increment(doid_t key, long value);
        void flush_leaderboard(std::string name);

    protected:
        virtual unsigned int time_until_next_task() = 0;
        virtual void calc_period_string() = 0;
        virtual int get_ld_flush_period() = 0;

        std::string m_period_string;

    private:
        void rotate(const boost::system::error_code& e);
        void leaderboard_task(const boost::system::error_code& e);

        boost::asio::deadline_timer m_ld_timer;
        Leaderboard* m_leaderboard;
};
