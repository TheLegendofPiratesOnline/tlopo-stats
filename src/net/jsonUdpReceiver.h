#pragma once

#include "udpReceiver.h"

#include <jansson.h>

class JSONUDPReceiver : public UDPReceiver {
    public:
        JSONUDPReceiver(boost::asio::io_service& io_service, const std::string& addr);

    protected:
        virtual void handle_data(const std::string& data);
        virtual void handle_packet(json_t* object) = 0;
};
