#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <vector>
#include <string>

//definition of a slot
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
        dirty(false) {}

    //Constructor for slot
    Slot(uint32_t tag, bool valid, uint32_t load_ts, uint32_t access_ts, bool dirty) :
        tag(tag),
        valid(valid),
        load_ts(load_ts),
        access_ts(access_ts),
        dirty(dirty) {}

};

//definition of a set; constituted of a vector of slots
struct Set {
    std::vector<Slot> slots;
};

//definition of a cache; 
//contains a vector of sets and information of the type of the cache
struct Cache {
    std::vector<Set> sets;
    uint32_t numSets;
    uint32_t numBlocks;
    uint32_t bytesOfMemory;
    std::string replace_strategy;
    std::string type_write_miss;
    std::string type_write_hit;
    uint32_t totalLoads;
    uint32_t totalStores;
    uint32_t totalLoadHits;
    uint32_t totalLoadMisses;
    uint32_t totalStoreHits;
    uint32_t totalStoreMisses;
    uint32_t totalCycles;
};

//a function to create cache
Cache create_cache(uint32_t numSets, uint32_t numBlocks, uint32_t bytesOfMemory, std::string replace_strategy, std::string type_write_miss, std::string type_write_hit);

//retreives the tag of an address
uint32_t get_tag(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory);
//retreives the index of an address
uint32_t get_index(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory);
//finds the cacheindex and setindex of a tag, return in tuple, <-1,-1> for one that is not found
std::tuple<int32_t, int32_t> find(const Cache& cache, uint32_t index, uint32_t in_tag);

//the function that loads an address to the cache, delegate to helper functions by condition
void load(uint32_t address, Cache& cache);
//the function for a load hit case
void handle_load_hit(Cache& cache, uint32_t indexSet, uint32_t indexSlot);
//the function for a load miss case in LRU mode
int handle_load_miss_LRU(Cache& cache, uint32_t indexSet, Slot newSlot);
//the function for a load miss case in FIFO mode
int handle_load_miss_FIFO(Cache& cache, uint32_t indexSet, Slot newSlot);

//stores an address back to cache
void store(uint32_t address, Cache& cache);

//handles input errors
int input_error_handling(int numSets, int numBlocks, int bytesOfMemory, std::string type_write_miss, std::string type_write_hit);







#endif