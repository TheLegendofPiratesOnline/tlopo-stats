#pragma once

#include "eventManager.h"

class EventListener {
    protected:
        void listen(const std::string& event_type, const event_callback_t& callback);
        void disconnect();
};
