#pragma once

#include "globals.h"

#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

class EventListener; // fwd decl

typedef struct
{
    std::string type;
    std::vector<doid_t> doIds;
    long value;
} Event;

typedef std::function<void(const Event&)> event_callback_t;

class EventManager final {
    public:
        void send(const Event& event);
        void listen(const std::string& event_type,
                    const EventListener* listener,
                    const event_callback_t& callback);
        void disconnect(const EventListener* listener);

        inline static EventManager* get_global_ptr() {
            static EventManager* e = new EventManager;
            return e;
        }

    private:
        EventManager();
        typedef std::vector<std::pair<std::string, event_callback_t>> callbacks_t;
        std::unordered_map<const EventListener*, callbacks_t> m_events; // {listener: [(event, callback)]}
};
