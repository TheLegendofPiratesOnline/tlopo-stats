#pragma once

#include "cache.h"
#include "collector/statCollectorBase.h"

class StatCollectorManager; // fwd decl

class CachedStatCollectorMap : public CachedMap<std::string, StatCollectorBase*>
{
    public:
        CachedStatCollectorMap(StatCollectorManager* mgr, const std::string& filename);

    protected:
        virtual void load();
        virtual void save();

    private:
        StatCollectorManager* m_mgr;
};
