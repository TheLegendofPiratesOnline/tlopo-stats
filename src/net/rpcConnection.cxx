#include "rpcConnection.h"

#include "collector/statCollectorManager.h"

#include <boost/bind.hpp>

#include <jansson.h>

#include <iostream>

RPCConnection::RPCConnection(tcp::socket* socket) : m_socket(socket), m_buffer(SOCK_BUFFER_SIZE)
{
    async_read_until(*m_socket, m_buffer,
                     '\n', boost::bind(&RPCConnection::handle_read, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void RPCConnection::handle_read(const boost::system::error_code& ec, size_t bytes)
{
    if (ec.value() != 0)
    {
        finish();
        return;
    }

    // Read request:
    m_buffer.commit(bytes);
    std::ostringstream ss;
    ss << &m_buffer;
    std::string data = ss.str();

    json_error_t error;
    json_t* args = json_loads(data.c_str(), 0, &error);
    if (!args)
    {
        std::cerr << "failed to load message " << data
                  << ": json_loads returned NULL: error on line "
                  << error.line << ": " <<  error.text << std::endl;
        finish();
        return;
    }

    if (!json_is_object(args))
    {
        std::cerr << "failed to load message " << data
                  << ": args is not an object" << std::endl;
        json_decref(args);
        finish();
        return;
    }

    json_t* j_method = json_object_get(args, "method");
    if (!j_method || !json_is_string(j_method))
    {
        std::cerr << "failed to load message " << data \
                  << ": invalid or missing method" << std::endl;
        json_decref(args);
        finish();
        return;
    }

    json_t* resp = json_object();
    json_t* result = nullptr;

    bool success = false;
    std::string error_str = "";

    std::string method = std::string(json_string_value(j_method));

    // Process request:
    if (method == "list")
    {
        success = true;
        StatCollectorManager::get_global_ptr()->write_json(&result);
    }

    else if (method == "remove")
    {
        auto j_name = json_object_get(args, "name");
        if (!j_name || !json_is_string(j_name))
        {
            error_str = "invalid or missing 'name' param";
        }

        else
        {
            std::string name = json_string_value(j_name);
            success = StatCollectorManager::get_global_ptr()->remove_collector(name);
            if (!success)
                error_str = "no such collector";
        }
    }

    else if (method == "add_incremental")
    {
        auto j_name = json_object_get(args, "name");
        auto j_event = json_object_get(args, "event");

        if (!j_name || !json_is_string(j_name))
        {
            error_str = "invalid or missing 'name' param";
        }

        else if (!j_event || !json_is_string(j_event))
        {
            error_str = "invalid or missing 'event' param";
        }

        else
        {
            std::string name = json_string_value(j_name);
            std::string event = json_string_value(j_event);
            success = StatCollectorManager::get_global_ptr()->add_incremental_collector(name, event);
            if (!success)
                error_str = name + " already exists";
        }
    }

    else if (method == "add_periodic")
    {
        auto j_name = json_object_get(args, "name");
        auto j_event = json_object_get(args, "event");
        auto j_period = json_object_get(args, "period");

        if (!j_name || !json_is_string(j_name))
        {
            error_str = "invalid or missing 'name' param";
        }

        else if (!j_event || !json_is_string(j_event))
        {
            error_str = "invalid or missing 'event' param";
        }

        else if (!j_period || !json_is_integer(j_period))
        {
            error_str = "invalid or missing 'period' param";
        }

        else
        {
            std::string name = json_string_value(j_name);
            std::string event = json_string_value(j_event);
            unsigned int period = json_integer_value(j_period);
            success = StatCollectorManager::get_global_ptr()->add_periodic_collector(name, event, period);
            if (!success)
                error_str = name + " already exists";
        }
    }

    else
    {
        error_str = "unknown method";
    }

    // Send response:
    json_object_set_new(resp, "success", json_boolean(success));
    if (result)
    {
        json_object_set_new(resp, "result", result);
    }

    else if (error_str.size())
    {
        json_object_set_new(resp, "error", json_string(error_str.c_str()));
    }

    char* dump = json_dumps(resp, 0);
    async_write(*m_socket, boost::asio::buffer(dump, strlen(dump)),
                boost::bind(&RPCConnection::handle_write, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    free(dump);

    // Cleanup:
    json_decref(resp);
    json_decref(args);
}

void RPCConnection::handle_write(const boost::system::error_code&, size_t)
{
    finish();
}

void RPCConnection::finish()
{
    delete m_socket;
    delete this;
}
