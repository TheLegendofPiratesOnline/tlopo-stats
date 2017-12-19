#include "collector/eventCollector.h"
#include "event/eventListener.h"
#include "avatar/avatarManager.h"

#include "database/dummyDatabase.h"
#include "collector/statCollector.h"
#include "collector/incrementalStatCollector.h"
#include "report/dailyReport.h"

static DailyReport* g_report = nullptr;

class TestListener : public EventListener
{
    public:
        TestListener()
        {
            listen("REPORT", [](const Event& e)
            {
                doid_t avId = e.doIds[0];
                long value = e.value;

                g_report->increment(avId, value);
            });
        }
};

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    AvatarManager::get_global_ptr()->start();

    EventCollector evcoll(io_service, "127.0.0.1");
    TestListener l;

    DummyDatabase db;

    {
      g_report = new DailyReport ("test", &db, io_service);
      g_report->start();
      std::cout << g_report->get_collection_name() << std::endl;
    }

    StatCollector coll("Pstat", "STAT",  &db, 10, io_service);
    coll.start();

    IncrementalStatCollector coll2("Istat", "STAT2", &db, io_service);
    coll2.start();

    io_service.run();
    return 0;
}
