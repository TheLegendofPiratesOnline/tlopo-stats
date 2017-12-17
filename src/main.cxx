#include "collector/eventCollector.h"
#include "event/eventListener.h"
#include "avatar/avatarManager.h"

class TestListener : public EventListener
{
    public:
        TestListener(AvatarManager* m)
        {
            listen("QUERY", [this, m](const Event& e)
            {
                std::cout << e.doIds[0] << ": " << m->get_guild_id(e.doIds[0]) << std::endl;
                std::cout << std::endl;
            });
        }
};

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    AvatarManager m;

    EventCollector evcoll(io_service, "127.0.0.1");
    TestListener l(&m);

    io_service.run();
    return 0;
}
