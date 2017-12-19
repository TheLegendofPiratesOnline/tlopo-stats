#include "avatarManager.h"

#include "constants.h"
#include "events.h"

AvatarManager::AvatarManager() : m_guild_ids(AVMGR_CACHE_FILENAME)
{
}

void AvatarManager::start()
{
    listen(EVENT_AV_ONLINE, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        m_guild_ids.set(avId, 0);
    });
    listen(EVENT_AV_GUILD, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        doid_t guildId = (doid_t)e.value;
        m_guild_ids.set(avId, guildId);
    });
    listen(EVENT_AV_OFFLINE, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        m_guild_ids.erase(avId);
    });
}

doid_t AvatarManager::get_guild_id(doid_t avId)
{
    return m_guild_ids.get(avId);
}
