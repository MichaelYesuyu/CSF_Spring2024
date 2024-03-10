#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <vector>
#include <string>

struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts;
    uint32_t access_ts;
    bool dirty;

    //Default constructor
    Slot() :
        tag(0),
        valid(false),
        load_ts(0),
        access_ts(0),
        dirty(0) {}

    //Constructor for slot
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
    uint32_t numSets;
    uint32_t numBlocks;
    uint32_t bytesOfMemory;
    std::string replace_strategy;
    std::string type_write_miss;
    std::string type_write_hit;
};

Cache create_cache(uint32_t numSets, uint32_t numBlocks, uint32_t bytesOfMemory, std::string replace_strategy, std::string type_write_miss, std::string type_write_hit);

uint32_t get_tag(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory);
uint32_t get_index(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory);
std::tuple<int32_t, int32_t> find(Cache cache, uint32_t index, uint32_t in_tag);

void load(uint32_t address, Cache cache, uint32_t simulation_timestep);
void handle_load_hit(Cache& cache, uint32_t indexSet, uint32_t indexSlot, uint32_t simulation_timestep);
void handle_load_miss_LRU(Cache& cache, uint32_t indexSet, Slot newSlot);
void handle_load_miss_FIFO(Cache& cache, uint32_t indexSet, Slot newSlot);

void store(uint32_t address, Cache& cache, uint32_t simulation_timestep);








#endif