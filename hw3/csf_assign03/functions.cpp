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
    cache.totalLoads = 0;
    cache.totalLoadHits = 0;
    cache.totalLoadMisses = 0;
    cache.totalStores = 0;
    cache.totalStoreHits = 0;
    cache.totalLoadMisses = 0;
    cache.totalCycles = 0;
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
    std::vector<Slot> lines = ((cache.sets)[index]).slots;
    for(int32_t i = 0; i < (int32_t)cache.numBlocks; i++){
        if(((lines[i]).tag == in_tag)&&(lines[i].valid)){
            return std::make_tuple(index, i);
        }        
    }
    return std::make_tuple(-1, -1);
}

void load(uint32_t address, Cache& cache){
    cache.totalLoads++;
    uint32_t index = get_index(address, cache.numSets, cache.bytesOfMemory);
    uint32_t tag = get_tag(address, cache.numSets, cache.bytesOfMemory);
    std::tuple<int32_t, int32_t> index_slot_pair = find(cache, index, tag);
    //cout << std::get<0>(index_slot_pair) << " " << std::get<1>(index_slot_pair) << endl;
    if (std::get<0>(index_slot_pair) != -1){
        handle_load_hit(cache, std::get<0>(index_slot_pair), std::get<1>(index_slot_pair));
        cache.totalLoadHits++;
        cache.totalCycles++;
        return;
    } else {
        Slot new_slot = Slot(tag, true, cache.totalCycles, cache.totalCycles, false);
        int status = 0;
        if(cache.replace_strategy == "lru"){
                status = handle_load_miss_LRU(cache, index, new_slot);
            } else if (cache.replace_strategy == "fifo") {
                status = handle_load_miss_FIFO(cache, index, new_slot);
            }
        cache.totalLoadMisses++;
        int cycleNum = 100 * (cache.bytesOfMemory / 4);
        cache.totalCycles += (cycleNum * (status + 1));
        return;
    }
}

void handle_load_hit(Cache& cache, uint32_t indexSet, uint32_t indexSlot){
    cache.sets[indexSet].slots[indexSlot].access_ts = cache.totalCycles;
}

//0 for not replacing a dirty slot, 1 for replacing a dirty slot (write to memory)
int handle_load_miss_LRU(Cache& cache, uint32_t indexSet, Slot newSlot){
    //If there are empty slots, insert value there
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(!cache.sets[indexSet].slots[i].valid){
            //cout << "i value: " << i << endl;
            cache.sets[indexSet].slots[i] = newSlot;
            return 0;
        }
    }
    //Replace using LRU if all the slots inside the set are full
    uint32_t min = cache.sets[indexSet].slots[0].access_ts;
    uint32_t index_LRU = 0;
    //Look for the least recently used slot and replace it with the new slot
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(cache.sets[indexSet].slots[i].access_ts < min){
            min = cache.sets[indexSet].slots[i].access_ts;
            index_LRU = i;
        }
    }
    int check_dirty;
    if(cache.sets[indexSet].slots[index_LRU].dirty){
        check_dirty = 1;
    } else {
        check_dirty = 0;
    }
    cache.sets[indexSet].slots[index_LRU] = newSlot;
    return check_dirty;
}

int handle_load_miss_FIFO(Cache& cache, uint32_t indexSet, Slot newSlot){
     //If there are empty slots, insert value there
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(!cache.sets[indexSet].slots[i].valid){
            cache.sets[indexSet].slots[i] = newSlot;
            return 0;
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
    int check_dirty;
    if(cache.sets[indexSet].slots[index_FIFO].dirty){
        check_dirty = 1;
    } else {
        check_dirty = 0;
    }
    cache.sets[indexSet].slots[index_FIFO] = newSlot;
    return check_dirty;
}

//tuple<_hit_count, miss_count, cycle> counts the increments of these three
void store(uint32_t address, Cache& cache){
    cache.totalStores++;
    uint32_t index = get_index(address, cache.numSets, cache.bytesOfMemory);
    uint32_t tag = get_tag(address, cache.numSets, cache.bytesOfMemory);
    std::tuple<int32_t, int32_t> index_slot_pair = find(cache, index, tag);
    if (std::get<0>(index_slot_pair) == -1){ //write miss
        cache.totalStoreMisses++;
        if(cache.type_write_miss == "write-allocate"){ //write_allocate
            load(address, cache);
            cache.totalLoadMisses--;
            cache.totalLoads--;
            if(cache.type_write_hit == "write-back"){
                index_slot_pair = find(cache, index, tag);
                cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].dirty = true;
                cache.totalCycles++;
                return;
            } else {
                int cycleNum = 100 * (cache.bytesOfMemory / 4);
                cache.totalCycles += cycleNum;
            }
        } else { //no_write_allocate
            int cycleNum = 100 * (cache.bytesOfMemory / 4);
            cache.totalCycles += cycleNum;
        }
    } else { //cache hit
        cache.totalStoreHits++;
        if(cache.type_write_hit == "write-through"){
            load(address, cache);
            cache.totalLoadHits--;
            cache.totalLoads--;
            int cycleNum = 100 * (cache.bytesOfMemory / 4);
            cache.totalCycles += cycleNum;
            return;
        } else { //write_back
            load(address, cache);
            cache.totalLoadHits--;
            cache.totalLoads--;
            index_slot_pair = find(cache, index, tag);
            cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].dirty = true;
            cache.totalCycles++;
            return;
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
    //Make sure types indicated are valid
    if(type_write_miss != "no-write-allocate" && type_write_miss != "write-allocate"){
        cerr << "Invalid type for write-miss" << endl;
        return 1;
    }
    if(type_write_hit != "write-through" && type_write_hit != "write-back"){
        cerr << "Invalid type for write-hit" << endl;
        return 1;
    }
    //Make sure no-write-allocate and write-back and not specified together
    if(type_write_miss == "no-write-allocate" && type_write_hit == "write-back"){
        cerr << "Cannot specify no-write-allocate with write-back" << endl;
        return 1;
    }

    return 0;
}
