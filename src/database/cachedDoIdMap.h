#pragma once

#include "cache.h"

class CachedDoIdMap : public CachedMap<doid_t, doid_t>
{
    public:
        CachedDoIdMap(const std::string& filename);

    private:
        virtual void load();
        virtual void save();
};
