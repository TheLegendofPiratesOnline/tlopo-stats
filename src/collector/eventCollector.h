#pragma once

#include "net/jsonUdpReceiver.h"

class EventCollector : public JSONUDPReceiver {
    public:
        EventCollector(boost::asio::io_service& io_service, const std::string& addr);

    protected:
        virtual void handle_packet(json_t* object);
};
