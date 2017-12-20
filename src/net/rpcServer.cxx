#include "rpcServer.h"
#include "rpcConnection.h"
#include "net/globals.h"
#include "net/util.h"

#include <boost/bind.hpp>

RPCServer::RPCServer(boost::asio::io_service& io_service, const std::string& addr) :
    m_io_service(io_service), m_acceptor(io_service)
{
    boost::system::error_code ec;

    auto addresses = resolve_address(addr, SOCK_DEFAULT_RPC_PORT, io_service, ec);
    if (ec.value() != 0)
    {
        std::cerr << "Bad address " << addr << std::endl;
        exit(1);
    }

    m_acceptor.open(addresses[0].protocol(), ec);
    m_acceptor.set_option(tcp::acceptor::reuse_address(true), ec);
    m_acceptor.bind(addresses[0], ec);
    m_acceptor.listen(tcp::socket::max_connections, ec);
    if (ec.value() != 0)
    {
        std::cerr << "Failed to listen on " << addr << std::endl;
        exit(1);
    }

    start_accept();
}

void RPCServer::start_accept()
{
    auto socket = new tcp::socket(m_io_service);
    m_acceptor.async_accept(*socket, boost::bind(&RPCServer::handle_accept, this, socket,
                                                boost::asio::placeholders::error));
}

void RPCServer::handle_accept(tcp::socket* socket, const boost::system::error_code& ec)
{
    if (ec)
    {
        delete socket;
        goto end;
    }

    new RPCConnection(socket); // deletes itself

end:
    start_accept();
}
