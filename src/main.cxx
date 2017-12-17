#include "net/jsonUdpReceiver.h"

class Test : public JSONUDPReceiver {
    public:
        Test(boost::asio::io_service& io_service, const std::string& addr) :
          JSONUDPReceiver(io_service, addr) {};

    protected:
        virtual void handle_packet(json_t* object)
        {
            std::cout << "A: " << json_string_value(json_object_get(object, "a")) << std::endl;
            std::cout << "B: " << json_string_value(json_object_get(object, "b")) << std::endl;
            std::cout << std::endl;
        }
};

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    Test t(io_service, "127.0.0.1");

    io_service.run();
    return 0;
}
