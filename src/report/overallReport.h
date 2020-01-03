#pragma once

#include "incrementalPeriodicReport.h"

class OverallReport : public IncrementalPeriodicReport {
    public:
        OverallReport(const std::string& name, Database* db,
                      boost::asio::io_service& io_service);
        virtual ~OverallReport();

    protected:
        virtual unsigned int time_until_next_task();
        virtual void calc_period_string();
};
