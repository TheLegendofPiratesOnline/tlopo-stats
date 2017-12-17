#include "net/jsonUdpReceiver.h"
#include "event/eventListener.h"

class Test : public JSONUDPReceiver {
    public:
        Test(boost::asio::io_service& io_service, const std::string& addr) :
          JSONUDPReceiver(io_service, addr), i(0) {};

    protected:
        virtual void handle_packet(json_t* object)
        {
            Event e;
            e.type = "MSG";
            e.value = i++;
            EventManager::get_global_ptr()->send(e);
        }

        int i;
};

class TestListener : public EventListener
{
    public:
        TestListener()
        {
            listen("MSG", [this](const Event& e)
            {
                std::cout << "event!" << std::endl;
                std::cout << e.value << std::endl;
                std::cout << std::endl;

                if (e.value == 2) disconnect();
            });
        }
};

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    Test t(io_service, "127.0.0.1");
    TestListener l;

    io_service.run();
    return 0;
}
