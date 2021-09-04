#pragma once

#include <fstream>
#include <iostream>
using namespace std;

class DLog {
    public:
        explicit DLog ()
        {
        }

        // Overload << operator using log type
        template<typename T> 
        DLog& operator<< (const T& data) 
        {
            ofstream outfile;
            outfile.open("log.txt", ios_base::app);
            if (outfile.is_open()) {
                outfile << data << "\n";
            }
        }

        DLog (const DLog &) = delete;
        DLog &operator= (const DLog &) = delete;

    private:
        std::string filename;
};
