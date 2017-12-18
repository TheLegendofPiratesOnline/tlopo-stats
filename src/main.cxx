#include "collector/eventCollector.h"
#include "event/eventListener.h"
#include "avatar/avatarManager.h"

#include "database/dummyDatabase.h"
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

    AvatarManager m;

    EventCollector evcoll(io_service, "127.0.0.1");
    TestListener l;

    DummyDatabase db;

    {
      g_report = new DailyReport ("test", &db, io_service);
      g_report->start();
      std::cout << g_report->get_collection_name() << std::endl;
    }

    io_service.run();
    return 0;
}
