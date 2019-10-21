#pragma once

#include "incrementalPeriodicReport.h"

class YearlyReport : public IncrementalPeriodicReport {
    public:
        YearlyReport(const std::string& name, Database* db,
                     boost::asio::io_service& io_service);
        virtual ~YearlyReport();

    protected:
        virtual unsigned int time_until_next_task();
        virtual void calc_period_string();
        inline virtual int get_ld_flush_period()
        {
            return 12 * 3600; // 12 hours
        }
};
