#pragma once

#include "event/eventListener.h"
#include "database/cache.h"
#include "stats_types.h"

class AvatarManager : private EventListener {
    public:
        void start();
        doid_t get_guild_id(doid_t avId);

        inline static AvatarManager* get_global_ptr() {
            static AvatarManager* am = new AvatarManager;
            return am;
        }

    private:
        AvatarManager();
        CachedDoIdMap m_guild_ids;
};
