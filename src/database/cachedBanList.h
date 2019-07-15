#pragma once

#include "cache.h"

class CachedBanList : public CachedList<doid_t>
{
    public:
        CachedBanList(const std::string& filename);

    private:
        virtual void load();
        virtual void save();
};
