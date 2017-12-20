#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class RPCServer final
{
    public:
        RPCServer(boost::asio::io_service& io_service, const std::string& addr);

    private:
        void start_accept();
        void handle_accept(tcp::socket* socket, const boost::system::error_code& ec);

        boost::asio::io_service& m_io_service;
        tcp::acceptor m_acceptor;
};
