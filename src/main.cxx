#include "collector/eventCollector.h"
#include "event/eventListener.h"
#include "avatar/avatarManager.h"

#include "database/mongoDatabase.h"
#include "database/dummyDatabase.h"
#include "collector/statCollectorManager.h"

static StatCollectorManager* g_mgr = nullptr;

class TestListener : public EventListener
{
    public:
        TestListener()
        {
            listen("DEL", [](const Event& e)
            {
                g_mgr->remove_collector("enemies_killed");
            });
        }
};

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    AvatarManager::get_global_ptr()->start();

    EventCollector evcoll(io_service, "127.0.0.1");
    TestListener l;

    MongoDatabase db("mongodb://localhost:27017/tlopo_stats");
    DummyDatabase dbX;

    g_mgr = new StatCollectorManager(&db, io_service);
    g_mgr->add_incremental_collector("enemies_killed", "ENEMY_KILLED");

    io_service.run();
    return 0;
}
