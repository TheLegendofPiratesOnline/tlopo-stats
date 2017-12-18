#pragma once

#include "incrementalPeriodicReport.h"

class DailyReport : public IncrementalPeriodicReport {
    public:
        DailyReport(const std::string& name, boost::asio::io_service& io_service);
        virtual ~DailyReport();

    protected:
        virtual unsigned int time_until_next_task();
        virtual void calc_period_string();
};
