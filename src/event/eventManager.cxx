#include "eventManager.h"
#include "eventListener.h"

EventManager::EventManager()
{
}

void EventManager::send(const Event& event)
{
    std::vector<event_callback_t> callbacks;

    for (auto& it : m_events)
    {
        for (auto& it2 : it.second)
        {
            if (it2.first == event.type)
            {
                auto callback = it2.second;
                callbacks.push_back(callback);
            }
        }
    }

    for (auto& cb : callbacks)
    {
        cb(event);
    }
}

void EventManager::listen(const std::string& event_type,
                          const EventListener* listener,
                          const event_callback_t& callback)
{
    m_events[listener].push_back({event_type, callback});
}

void EventManager::disconnect(const EventListener* listener)
{
    m_events.erase(listener);
}
