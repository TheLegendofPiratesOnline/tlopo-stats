#include "udpReceiver.h"

#include "net/util.h"

#include <iostream>

using boost::asio::ip::udp;

UDPReceiver::UDPReceiver(boost::asio::io_service& io_service, const std::string& addr)
{
    boost::system::error_code ec;

    auto addresses = resolve_address(addr, SOCK_DEFAULT_PORT, io_service, ec);
    if (ec.value() != 0)
    {
        std::cerr << "Bad address " << addr << std::endl;
        exit(1);
    }

    m_sock = new udp::socket(io_service, udp::endpoint(addresses[0].address(),
                                                       addresses[0].port()));
    start_receive();
}

void UDPReceiver::start_receive()
{
    m_sock->async_receive_from(boost::asio::buffer(m_buffer, SOCK_BUFFER_SIZE),
                               m_remote, boost::bind(&UDPReceiver::handle_receive, this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));
}

void UDPReceiver::handle_receive(const boost::system::error_code &ec, std::size_t bytes)
{
    if (ec.value() != 0)
    {
        start_receive();
        return;
    }

    handle_data(std::string(m_buffer, bytes));
    start_receive();
}
