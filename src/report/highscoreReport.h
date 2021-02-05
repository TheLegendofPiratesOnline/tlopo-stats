#pragma once

#include "report.h"

class HighscoreReport : public Report {
    public:
        HighscoreReport(const std::string& name, Database* db,
                        boost::asio::io_service& io_service);
        virtual ~HighscoreReport();

        virtual void start();

        void set(doid_t key, long value);

    private:
        std::unordered_map<doid_t, long> m_entries;
};
