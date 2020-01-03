#pragma once

#include "event/eventListener.h"
#include "database/database.h"
#include "globals.h"

class AvatarManager : private EventListener {
    public:
        void init(Database* db);
        doid_t get_guild_id(doid_t avId);

        inline static AvatarManager* get_global_ptr() {
            static AvatarManager* am = new AvatarManager;
            return am;
        }

    private:
        AvatarManager();

        guild_map_t m_guild_ids;
};
