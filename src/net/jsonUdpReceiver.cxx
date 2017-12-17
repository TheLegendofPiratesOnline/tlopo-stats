#include "jsonUdpReceiver.h"

JSONUDPReceiver::JSONUDPReceiver(boost::asio::io_service& io_service, const std::string& addr) :
    UDPReceiver(io_service, addr)
{
}

void JSONUDPReceiver::handle_data(const std::string& data)
{
     json_error_t error;
     json_t* args = json_loads(data.c_str(), 0, &error);
     if (!args)
     {
         std::cerr << "failed to load message " << data \
                   << ": json_loads returned NULL: error on line " \
                   << error.line << ": " <<  error.text << std::endl;
         return;
     }

     if (!json_is_object(args))
     {
         std::cerr << "failed to load message " << data \
                   << ": args is not an object" << std::endl;
         json_decref(args);
         return;
     }

     handle_packet(args);
     json_decref(args);
}
