#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "net/globals.h"

class UDPReceiver {
    public:
        UDPReceiver(boost::asio::io_service& io_service, const std::string& addr);

    protected:
        virtual void handle_data(const std::string& data) = 0;

    private:
        void start_receive();
        void handle_receive(const boost::system::error_code &ec, std::size_t bytes);

        boost::asio::ip::udp::socket* m_sock;
        boost::asio::ip::udp::endpoint m_remote;
        char m_buffer[SOCK_BUFFER_SIZE];
};
