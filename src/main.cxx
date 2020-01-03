#include "collector/eventCollector.h"
#include "avatar/avatarManager.h"

#include "database/database.h"
#include "collector/statCollectorManager.h"
#include "net/rpcServer.h"

#include <iostream>

void usage(const std::string& error = "")
{
    std::cerr << "tlopostats [options]" << std::endl;
    std::cerr << "options:" << std::endl;
    std::cerr << std::endl;
    std::cerr << "--listen addr: address to listen on (default: 127.0.0.1:8963)" << std::endl;
    std::cerr << "--rpc addr: address to listen on (default: 127.0.0.1:8964)" << std::endl;
    std::cerr << "--dummy-db: use DummyDatabase backend instead of MongoDatabase" << std::endl;
    std::cerr << "--redis-db addr: Redis IP, port prefix (default: 127.0.0.1, 6379, tlopo_stats_test)" << std::endl;
    if (error.size())
    {
        std::cerr << std::endl;
        std::cerr << error << std::endl;
    }

    exit(1);
}

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    // Parse argv
    bool use_dummy_db = false;
    std::string addr = "127.0.0.1";
    std::string rpc_addr = "127.0.0.1";

    std::string db_addr = "127.0.0.1";
    std::string db_prefix = "tlopo_stats_test";
    int db_port = 6379;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--dummy-db") == 0)
        {
            use_dummy_db = true;
        }

        else if (strcmp(argv[i], "--listen") == 0)
        {
            if (i == argc - 1)
            {
                usage("--listen takes 1 argument");
                return 1;
            }

            addr = std::string(argv[++i]);
        }

        else if (strcmp(argv[i], "--rpc") == 0)
        {
            if (i == argc - 1)
            {
                usage("--rpc takes 1 argument");
                return 1;
            }

            rpc_addr = std::string(argv[++i]);
        }

        else if (strcmp(argv[i], "--redis-db") == 0)
        {
            if (i == argc - 3)
            {
                usage("--db-addr takes 3 arguments");
                return 1;
            }

            db_addr = std::string(argv[++i]);
            db_port = atoi(argv[++i]);
            db_prefix = std::string(argv[++i]);
        }

        else
        {
            usage();
            return 1;
        }
    }

    // Create the DB
    Database* db;

    if (use_dummy_db)
    {
        std::cout << "Using DummyDatabase backend" << std::endl;
        db = get_dummy_db();
    }

    else
    {
        std::cout << "Using Redis backend, db_addr = " << db_addr << ":" << db_port << std::endl;
        db = get_redis_db(db_addr, db_port, db_prefix);
    }

    // Init AvatarManager
    AvatarManager::get_global_ptr()->init(db);

    // Init StatCollectorManager
    StatCollectorManager::get_global_ptr()->init(db, io_service);

    // Start EventCollector
    std::cout << "Listening on " << addr << std::endl;
    EventCollector evcoll(io_service, addr);

    // Start the RPC server
    std::cout << "RPC: Listening on " << rpc_addr << std::endl;
    RPCServer rpc(io_service, rpc_addr);

    // Run
    io_service.run();
    return 0;
}
