#include "eventListener.h"

void EventListener::listen(const std::string& event_type, const event_callback_t& callback)
{
    EventManager::get_global_ptr()->listen(event_type, this, callback);
}

void EventListener::disconnect()
{
    EventManager::get_global_ptr()->disconnect(this);
}
