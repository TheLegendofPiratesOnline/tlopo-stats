#pragma once

#include "report.h"
#include "stats_types.h"

#include <unordered_map>

class IncrementalPeriodicReport : public Report {
    public:
        IncrementalPeriodicReport(const std::string& name, Database* db,
                                  boost::asio::io_service& io_service);
        virtual ~IncrementalPeriodicReport();

        virtual void start();

        virtual std::string get_collection_name();
        void increment(doid_t key, long value);
        void flush();

    protected:
        virtual unsigned int time_until_next_task() = 0;
        virtual void calc_period_string() = 0;

        std::string m_period_string;

    private:
        void rotate(const boost::system::error_code& e);

        std::unordered_map<doid_t, long> m_temp_cache;
};
