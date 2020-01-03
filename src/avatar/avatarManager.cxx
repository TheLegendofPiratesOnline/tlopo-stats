#include "avatarManager.h"

#include "globals.h"

AvatarManager::AvatarManager()
{
}

void AvatarManager::init(Database* db)
{
    db->get_guild_map(m_guild_ids);

    listen(EVENT_AV_ONLINE, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        m_guild_ids[avId] = 0;
    });
    listen(EVENT_AV_GUILD, [this, db](const Event& e) {
        doid_t avId = e.doIds[0];
        doid_t guildId = (doid_t)e.value;
        m_guild_ids[avId] = guildId;
        db->add_to_guild_map(avId, guildId);
    });
    listen(EVENT_AV_OFFLINE, [this](const Event& e) {
        doid_t avId = e.doIds[0];
        m_guild_ids.erase(avId);
    });
}

doid_t AvatarManager::get_guild_id(doid_t avId)
{
    auto it = m_guild_ids.find(avId);
    if (it == m_guild_ids.end())
        return 0;

    return it->second;
}
