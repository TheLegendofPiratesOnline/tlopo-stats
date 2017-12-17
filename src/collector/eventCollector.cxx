#include "eventCollector.h"

#include "event/eventManager.h"

EventCollector::EventCollector(boost::asio::io_service& io_service,
                               const std::string& addr) : JSONUDPReceiver(io_service, addr)
{
}

void EventCollector::handle_packet(json_t* object)
{
    json_t* j_event = json_object_get(object, "event");
    json_t* j_doIds = json_object_get(object, "doIds");
    json_t* j_value = json_object_get(object, "value");

    if (!j_event || !json_is_string(j_event))
    {
        std::cerr << "received msg without 'event' arg!" << std::endl;
        return;
    }

    Event e;
    e.type = json_string_value(j_event);
    e.value = 0;

    if (j_value && json_is_integer(j_value))
    {
        e.value = json_integer_value(j_value);
    }

    if (j_doIds && json_is_array(j_doIds))
    {
        for (json_int_t i = 0; i < json_array_size(j_doIds); i++)
        {
            json_t* item = json_array_get(j_doIds, i);
            if (item && json_is_integer(item))
                e.doIds.push_back(json_integer_value(item));
        }
    }

    EventManager::get_global_ptr()->send(e);
}
