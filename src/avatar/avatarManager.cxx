#include "avatarManager.h"

#include "events.h"

AvatarManager::AvatarManager()
{
}

void AvatarManager::start()
{
    listen(EVENT_AV_ONLINE, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        Avatar av;
        av.avId = avId;
        av.guildId = 0;
        m_avatars[avId] = av;
    });
    listen(EVENT_AV_GUILD, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        doid_t guildId = (doid_t)e.value;
        if (m_avatars.count(avId)) {
            m_avatars[avId].guildId = guildId;
        }
    });
    listen(EVENT_AV_OFFLINE, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        m_avatars.erase(avId);
    });
}

doid_t AvatarManager::get_guild_id(doid_t avId)
{
    if (!m_avatars.count(avId))
        return 0;

    return m_avatars[avId].guildId;
}
