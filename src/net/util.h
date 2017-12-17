#pragma once

#include <boost/asio.hpp>

std::vector<boost::asio::ip::tcp::endpoint> resolve_address(
    const std::string &hostspec, uint16_t port,
    boost::asio::io_service &io_service, boost::system::error_code &ec);
