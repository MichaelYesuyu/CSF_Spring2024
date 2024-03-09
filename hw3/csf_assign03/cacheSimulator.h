#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H

#include <stdint.h>
#include <vector>

struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts;
    uint32_t access_ts;
    bool dirty;

    Slot(uint32_t tag, bool valid, uint32_t load_ts, uint32_t access_ts, bool dirty) :
        tag(tag),
        valid(valid),
        load_ts(load_ts),
        access_ts(access_ts),
        dirty(dirty) {}

};

struct Set {
    std::vector<Slot> slots;
};

struct Cache {
    std::vector<Set> sets;
};







#endif