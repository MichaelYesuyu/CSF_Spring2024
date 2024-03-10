#include "functions.h"
#include <iostream>
#include <tuple>
#include <string>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using std::get;
using std::cerr;

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

int log2int(uint32_t value) {
    int log = 0;
    while (value >>= 1) ++log;
    return log;
}

uint32_t get_tag(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory){
    int indexSize = log2int(numSets);
    int offset = log2int(bytesOfMemory);
    int totaloffset = indexSize + offset;
    uint32_t tag = address >> totaloffset;
    return tag;
}

uint32_t get_index(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory){
    int offset = log2int(bytesOfMemory);
    uint32_t tag_index = address >> offset;
    uint32_t index = tag_index & (numSets - 1);
    return index;
}

//Returns tuple (index of set, index of slot)
std::tuple<int32_t, int32_t> find(const Cache &cache, uint32_t index, uint32_t in_tag){
    for(int32_t i = 0; i < (int32_t)cache.numSets; i++){
        if(i == (int32_t)index){
            std::vector<Slot> lines = ((cache.sets)[i]).slots;
            for(int32_t j = 0; j < (int32_t)cache.numBlocks; j++){
                if(((lines[j]).tag == in_tag)&&(lines[j].valid)){
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
            assert(load(address, cache, simulation_timestep) == -1);
            return -1;
        } else { //no_write_allocate
           // no_write_allocate();
           //do nothing?
           return -1;
        }
    } else { //cache hit
        if(cache.type_write_hit == "write-through"){
          return load(address, cache, simulation_timestep);
        } else { //write_back
           if(cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].dirty){
            return load(address, cache, simulation_timestep);
           } else {
            cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].dirty = true;
            return 1;
           }
        }
    }
}

int input_error_handling(int numSets, int numBlocks, int bytesOfMemory, string type_write_miss, string type_write_hit){
    //Make sure number of sets is positive
    if(numSets < 0){
        cerr << "Number of sets must be positive" << endl;
        return 1;
    }
    //Make sure number of blocks is positive
    if(numBlocks < 0){
        cerr << "Number of blocks must be positive" << endl;
        return 1;
    }
    //Make sure block size is at least 4
    if(bytesOfMemory < 4){
        cerr << "Block size must be at least 4" << endl;
        return 1;
    }
    //Make sure that block size is a power of 2
    int num = bytesOfMemory & (bytesOfMemory - 1);
    if(num != 0){
        cerr << "Block size must be a power of 2" << endl;
        return 1;
    }
    //Make sure that number of sets is a power of 2
    int num2 = numSets & (numSets - 1);
    if(num2 != 0){
        cerr << "Number of sets must be a power of 2" << endl;
        return 1;
    }
    //Make sure no-write-allocate and write-back and not specified together
    if(type_write_miss == "no-write-allocate" && type_write_hit == "write-back"){
        cerr << "Cannot specify no-write-allocate with write-back" << endl;
        return 1;
    }

    return 0;
}


void write_allocate(){}

void no_write_allocate(){}

void write_through(){}

void write_back(){}