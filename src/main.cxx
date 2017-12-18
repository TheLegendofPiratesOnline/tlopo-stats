#include "collector/eventCollector.h"
#include "event/eventListener.h"
#include "avatar/avatarManager.h"

#include "report/dailyReport.h"
#include "report/monthlyReport.h"
#include "report/yearlyReport.h"
#include "report/periodicReport.h"

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

    {
      Report* r = new DailyReport ("test", io_service);
      r->start();
      std::cout << r->get_db_name() << std::endl;
    }
    {
      Report* r = new MonthlyReport ("test", io_service);
      r->start();
      std::cout << r->get_db_name() << std::endl;
    }
    {
      Report* r = new YearlyReport ("test", io_service);
      r->start();
      std::cout << r->get_db_name() << std::endl;
    }
    {
      Report* r = new PeriodicReport ("periodic", 5, io_service);
      r->start();
      std::cout << r->get_db_name() << std::endl;
    }

    io_service.run();
    return 0;
}
