#pragma once

#include "rpcServer.h"
#include "net/globals.h"

class RPCConnection final
{
    public:
        RPCConnection(tcp::socket* socket);

    private:
        void handle_read(const boost::system::error_code& ec, size_t bytes);
        void handle_write(const boost::system::error_code&, size_t);
        void finish();

        tcp::socket* m_socket;
        boost::asio::streambuf m_buffer;
};
