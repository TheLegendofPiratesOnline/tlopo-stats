#include "util.h"

using boost::asio::ip::tcp;
using boost::asio::ip::address;

bool split_port(std::string &ip, uint16_t &port)
{
    size_t last_colon = ip.rfind(':');
    if(last_colon == std::string::npos) {
        return true;
    }

    // If the input contains *multiple* colons, it's an IPv6 address.
    // We ignore these unless the IPv6 address is bracketed and the port
    // specification occurs outside of the brackets.
    // (e.g. "[::]:1234")
    if(std::count(ip.begin(), ip.end(), ':') > 1) {
        // Yep, we're assuming IPv6. Let's see if the last colon has a
        // ']' before it.
        // Note that this still can lead to weird inputs getting
        // through, but that'll get caught by a later parsing step. :)
        if(ip[last_colon - 1] != ']') {
            return true;
        }
    }

    try {
        port = std::stoi(ip.substr(last_colon + 1));
        ip = ip.substr(0, last_colon);
    } catch(std::invalid_argument) {
        return false;
    }

    return true;
}

address parse_address(const std::string &ip, boost::system::error_code &ec)
{
    if(ip[0] == '[' && ip[ip.length() - 1] == ']') {
        return address::from_string(ip.substr(1, ip.length() - 2), ec);
    } else {
        return address::from_string(ip, ec);
    }
}

std::vector<tcp::endpoint> resolve_address(
    const std::string &hostspec, uint16_t port,
    boost::asio::io_service &io_service, boost::system::error_code &ec)
{
    std::vector<tcp::endpoint> ret;

    std::string host = hostspec;

    if(!split_port(host, port)) {
        return ret;
    }

    address addr = parse_address(host, ec);
    if(ec.value() == 0) {
        tcp::endpoint ep(addr, port);
        ret.push_back(ep);
    } else {
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(host, std::to_string(port));

        tcp::resolver::iterator it = resolver.resolve(query, ec);
        tcp::resolver::iterator end;

        while(it != end) {
            ret.push_back(*it++);
        }
    }

    return ret;
}
