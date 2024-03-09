#include "functions.h"
#include <cmath>
#include <tuple>

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

uint32_t get_tag(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory){
    int indexSize = std::log2(numSets);
    int offset = std::log2(bytesOfMemory);
    int totaloffset = indexSize + offset;
    uint32_t tag = address >> totaloffset;
    return tag;
}

uint32_t get_index(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory){
    int offset = std::log2(bytesOfMemory);
    uint32_t tag_index = address >> offset;
    uint32_t index = tag_index & (numSets - 1);
    return index;
}

void load(uint32_t address, Cache cache){

}

std::tuple<int32_t, int32_t> find(Cache cache, uint32_t index, uint32_t in_tag){
    for(int32_t i = 0; i < (int32_t)cache.sets.size(); i++){
        if(i == (int32_t)index){
            std::vector<Slot> lines = ((cache.sets)[i]).slots;
            for(int32_t j = 0; j < (int32_t)lines.size(); j++){
                if((lines[j]).tag == in_tag){
                    return std::make_tuple(i, j);
                }
            }
        }
        return std::make_tuple(-1, -1);
    }
}

void store(Cache& cache, uint32_t address)

void write_allocate()

void no_write_allocate()

void write_through()

void write_back()