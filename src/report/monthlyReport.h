#pragma once

#include "incrementalPeriodicReport.h"

class MonthlyReport : public IncrementalPeriodicReport {
    public:
        MonthlyReport(const std::string& name, Database* db,
                      boost::asio::io_service& io_service);
        virtual ~MonthlyReport();

    protected:
        virtual unsigned int time_until_next_task();
        virtual void calc_period_string();
        inline virtual int get_ld_flush_period()
        {
            return 3600; // 1 hour
        }
};
