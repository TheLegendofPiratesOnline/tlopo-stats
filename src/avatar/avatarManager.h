#pragma once

#include "event/eventListener.h"
#include "stats_types.h"

#include <unordered_map>

typedef struct {
  doid_t avId;
  doid_t guildId;
} Avatar;

class AvatarManager : private EventListener {
    public:
        AvatarManager();
        doid_t get_guild_id(doid_t avId);

    private:
        std::unordered_map<doid_t, Avatar> m_avatars;
};
