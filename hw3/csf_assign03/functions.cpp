#include "functions.h"
#include <cmath>

Cache create_cache(int numSets, int numBlocks){
    Cache cache;
    for (int i = 0; i < numSets; i++){
        Set set;
        for (int j = 0; j < numBlocks; j++){
            Slot slot;
            set.slots.push_back(slot);
        }
        cache.sets.push_back(set);
    }
    return cache;
}

uint32_t get_tag(uint32_t address, int numSets, int bytesOfMemory){
    int indexSize = std::log2(numSets);
    int offset = std::log2(bytesOfMemory);
    int totaloffset = indexSize + offset;
    uint32_t tag = address >> totaloffset;
    return tag;
}

uint32_t get_index(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory){
    
}