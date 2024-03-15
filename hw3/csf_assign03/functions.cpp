#include "functions.h"
#include <iostream>
#include <iostream>
#include <fstream>
#include <tuple>
#include <string>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using std::get;
using std::cerr;

//a function to create cache
Cache create_cache(uint32_t numSets, uint32_t numBlocks, uint32_t bytesOfMemory, string replace_strategy, string type_write_miss, string type_write_hit){
    Cache cache; //initialize cache with default values
    for (int i = 0; i < (int)numSets; i++){
        Set set;
        for (int j = 0; j < (int)numBlocks; j++){
            Slot slot;
            set.slots.push_back(slot);
        } //create a set with target number of slots
        cache.sets.push_back(set);
    }
    //load in parameters of a cache
    cache.numSets = numSets;
    cache.numBlocks = numBlocks;
    cache.bytesOfMemory = bytesOfMemory;
    cache.replace_strategy = replace_strategy;
    cache.type_write_miss = type_write_miss;
    cache.type_write_hit = type_write_hit;
    //set all countings to 0
    cache.totalLoads = 0;
    cache.totalLoadHits = 0;
    cache.totalLoadMisses = 0;
    cache.totalStores = 0;
    cache.totalStoreHits = 0;
    cache.totalStoreMisses = 0;
    cache.totalCycles = 0;
    return cache;
}

//compute the log 2 value of integers, return also in integer
int log2int(uint32_t value) {
    int log = 0;
    while (value >>= 1) ++log;
    return log;
}

//retreives the tag of an address
uint32_t get_tag(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory){
    //the number of bits for the index part
    int indexSize = log2int(numSets);
    //the number of bits for the offset part
    int offset = log2int(bytesOfMemory);
    int totaloffset = indexSize + offset;
    //the rest of the address is the tag part
    uint32_t tag = address >> totaloffset;
    return tag;
}

//retreives the index of an address
uint32_t get_index(uint32_t address, uint32_t numSets, uint32_t bytesOfMemory){
    //the number of bits for the offset part
    int offset = log2int(bytesOfMemory);
    uint32_t tag_index = address >> offset;
    //consider only the middle part by bitwise and
    uint32_t index = tag_index & (numSets - 1);
    return index;
}

//Returns tuple (index of set, index of slot)
std::tuple<int32_t, int32_t> find(const Cache &cache, uint32_t index, uint32_t in_tag){
    //only need to traverse the target set
    const std::vector<Slot> &lines = ((cache.sets)[index]).slots;
    for(uint32_t i = 0; i < cache.numBlocks; i++){
        if(((lines[i]).tag == in_tag)&&(lines[i].valid)){
            return std::make_tuple(index, i);
        }        
    }
    //if not found, return (-1, -1)
    return std::make_tuple(-1, -1);
}

//the function that loads an address to the cache, delegate to helper functions by condition
void load(uint32_t address, Cache& cache){
    //first increment total loads
    cache.totalLoads++;
    //get the index, tag, and index of slot / set
    uint32_t index = get_index(address, cache.numSets, cache.bytesOfMemory);
    uint32_t tag = get_tag(address, cache.numSets, cache.bytesOfMemory);
    std::tuple<int32_t, int32_t> index_slot_pair = find(cache, index, tag);
    //if we did find the target in the cache, then delegate to load_hit
    if (std::get<0>(index_slot_pair) != -1){
        handle_load_hit(cache, std::get<0>(index_slot_pair), std::get<1>(index_slot_pair));
        //in this case, load into cache requires 1 cycle
        cache.totalLoadHits++;
        cache.totalCycles++;
        return;
    } else { //if we did not find it
        //new slot to be put into cache
        Slot new_slot = Slot{tag, true, cache.totalCycles, cache.totalCycles, false};
        //use status to track if we modified a dirty slot or not. If so, need to write to memory
        int status = 0;
        //delegate to helper functions
        if(cache.replace_strategy == "lru"){
                status = handle_load_miss_LRU(cache, index, new_slot);
            } else if (cache.replace_strategy == "fifo") {
                status = handle_load_miss_FIFO(cache, index, new_slot);
            }
        cache.totalLoadMisses++;
        int cycleNum = 25 * cache.bytesOfMemory;
        //if overwrote dirty slot, need to perform one more write to memory
        cache.totalCycles += (cycleNum * (status + 1));
        return;
    }
}

//the function for a load hit case
void handle_load_hit(Cache& cache, uint32_t indexSet, uint32_t indexSlot){
    //update the target's access time
    cache.sets[indexSet].slots[indexSlot].access_ts = cache.totalCycles;
}

//the function for a load miss case in LRU mode
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
    //if this slot is dirty, return 1, return 0 otherwise
    int check_dirty;
    if(cache.sets[indexSet].slots[index_LRU].dirty){
        check_dirty = 1;
    } else {
        check_dirty = 0;
    }
    cache.sets[indexSet].slots[index_LRU] = newSlot;
    return check_dirty;
}

//the function for a load miss case in FIFO mode
//0 for not replacing a dirty slot, 1 for replacing a dirty slot (write to memory)
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
    //if this slot is dirty, return 1, return 0 otherwise
    int check_dirty;
    if(cache.sets[indexSet].slots[index_FIFO].dirty){
        check_dirty = 1;
    } else {
        check_dirty = 0;
    }
    cache.sets[indexSet].slots[index_FIFO] = newSlot;
    return check_dirty;
}

//stores an address back to cache
//call load() in the case when we need to load slots to the cache
void store(uint32_t address, Cache& cache){
    //first increment total loads
    cache.totalStores++;
    //get the index, tag, and index of slot / set
    uint32_t index = get_index(address, cache.numSets, cache.bytesOfMemory);
    uint32_t tag = get_tag(address, cache.numSets, cache.bytesOfMemory);
    std::tuple<int32_t, int32_t> index_slot_pair = find(cache, index, tag);
    if (std::get<0>(index_slot_pair) == -1){ //write miss
        cache.totalStoreMisses++;
        if(cache.type_write_miss == "write-allocate"){ //write_allocate
            //first load the slot to cache, and restore Load count
            load(address, cache);
            cache.totalLoadMisses--;
            cache.totalLoads--;
            if(cache.type_write_hit == "write-back"){ //write back write allocate
                //find again because we modified the cache
                index_slot_pair = find(cache, index, tag);
                cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].dirty = true;
                cache.totalCycles++;
                return;
            } else { //write through write allocate
                cache.totalCycles += 25 * cache.bytesOfMemory;
            }
        } else { //no_write_allocate
            cache.totalCycles += 25 * cache.bytesOfMemory;
        }
    } else { //cache hit
        cache.totalStoreHits++;
        if(cache.type_write_hit == "write-through"){ //write through
            cache.totalCycles++;
            cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].access_ts = cache.totalCycles;
            cache.totalCycles += 25 * cache.bytesOfMemory;
            return;
        } else { //write_back
            cache.totalCycles++;
            cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].dirty = true;
            cache.sets[get<0>(index_slot_pair)].slots[get<1>(index_slot_pair)].access_ts = cache.totalCycles;
            cache.totalCycles++;
            return;
           }
        }
    
}

//handles input errors
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
