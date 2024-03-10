#include "functions.h"
#include <iostream>
#include <cmath>
#include <tuple>
#include <string>

using std::string;
using std::cout;
using std::endl;

Cache create_cache(uint32_t numSets, uint32_t numBlocks, uint32_t bytesOfMemory, string replace_strategy, string type_write_miss, string type_write_hit){
    Cache cache;
    for (int i = 0; i < (int)numSets; i++){
        Set set;
        for (int j = 0; j < (int)numBlocks; j++){
            Slot slot;
            set.slots.push_back(slot);
        }
        cache.sets.push_back(set);
    }
    cache.numSets = numSets;
    cache.numBlocks = numBlocks;
    cache.bytesOfMemory = bytesOfMemory;
    cache.replace_strategy = replace_strategy;
    cache.type_write_miss = type_write_miss;
    cache.type_write_hit = type_write_hit;
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

//Returns tuple (index of set, index of slot)
std::tuple<int32_t, int32_t> find(const Cache &cache, uint32_t index, uint32_t in_tag){
    for(int32_t i = 0; i < (int32_t)cache.sets.size(); i++){
        if(i == (int32_t)index){
            std::vector<Slot> lines = ((cache.sets)[i]).slots;
            for(int32_t j = 0; j < (int32_t)lines.size(); j++){
                if((lines[j]).tag == in_tag && lines[j].valid){
                    return std::make_tuple(i, j);
                }
            }
        }
    }
    return std::make_tuple(-1, -1);
}

int load(uint32_t address, Cache& cache, uint32_t simulation_timestep){
    uint32_t index = get_index(address, cache.numSets, cache.bytesOfMemory);
    uint32_t tag = get_tag(address, cache.numSets, cache.bytesOfMemory);
    std::tuple<int32_t, int32_t> index_slot_pair = find(cache, index, tag);
    //cout << std::get<0>(index_slot_pair) << " " << std::get<1>(index_slot_pair) << endl;
    if (std::get<0>(index_slot_pair) != -1){
        handle_load_hit(cache, std::get<0>(index_slot_pair), std::get<1>(index_slot_pair), simulation_timestep);
        return 1;
    } else {
        Slot new_slot = Slot(tag, true, simulation_timestep, simulation_timestep, false);
        if(cache.replace_strategy == "lru"){
                handle_load_miss_LRU(cache, index, new_slot);
                return -1;
            } else if (cache.replace_strategy == "fifo") {
                handle_load_miss_FIFO(cache, index, new_slot);
                return -1;
            } else { //error
                return 0;
            }
    }
}

void handle_load_hit(Cache& cache, uint32_t indexSet, uint32_t indexSlot, uint32_t simulation_timestep){
    cache.sets[indexSet].slots[indexSlot].access_ts = simulation_timestep;
}

void handle_load_miss_LRU(Cache& cache, uint32_t indexSet, Slot newSlot){
    //If there are empty slots, insert value there
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(!cache.sets[indexSet].slots[i].valid){
            //cout << "i value: " << i << endl;
            cache.sets[indexSet].slots[i] = newSlot;
            return;
        }
    }
    //cout << "REACHED!" << endl;
    //Replace using LRU if all the slots inside the set are full
    uint32_t min = UINT32_MAX;
    int index_LRU = 0;
    //Look for the least recently used slot and replace it with the new slot
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(cache.sets[indexSet].slots[i].access_ts < min){
            min = cache.sets[indexSet].slots[i].access_ts;
            index_LRU = i;
        }
    }
    cache.sets[indexSet].slots[index_LRU] = newSlot;
}

void handle_load_miss_FIFO(Cache& cache, uint32_t indexSet, Slot newSlot){
     //If there are empty slots, insert value there
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(!cache.sets[indexSet].slots[i].valid){
            cache.sets[indexSet].slots[i] = newSlot;
            return;
        }
    }
    //Replace using FIFO if all the slots inside the set are full
    uint32_t min = UINT32_MAX;
    int index_FIFO = 0;
    //Look for the earliest loaded slot and replace it with the new slot
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(cache.sets[indexSet].slots[i].load_ts < min){
            min = cache.sets[indexSet].slots[i].load_ts;
            index_FIFO = i;
        }
    }
    cache.sets[indexSet].slots[index_FIFO] = newSlot;
}

int store(uint32_t address, Cache& cache, uint32_t simulation_timestep){
    uint32_t index = get_index(address, cache.numSets, cache.bytesOfMemory);
    uint32_t tag = get_tag(address, cache.numSets, cache.bytesOfMemory);
    std::tuple<int32_t, int32_t> index_slot_pair = find(cache, index, tag);

    if (std::get<0>(index_slot_pair) == -1){ //cache miss
        if(cache.type_write_miss == "write-allocate"){
            return load(address, cache, simulation_timestep);
        } else { //no_write_allocate
           // no_write_allocate();
           //do nothing?
           return 1;
        }
    } else { //cache hit
        if(cache.type_write_hit == "write-through"){
          return load(address, cache, simulation_timestep);
        } else { //write_back
           if(cache.sets[index].slots[tag].dirty){
            return load(address, cache, simulation_timestep);
           } else {
            cache.sets[index].slots[tag].dirty = true;
            return 1;
           }
        }
    }
}

void write_allocate(){}

void no_write_allocate(){}

void write_through(){}

void write_back(){}