#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <tuple>
#include "functions.h"

using std::ifstream;
using std::stoi;
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::stringstream;
using std::cerr;
using std::cin;
using std::tuple;
using std::get;

//Sample command line input
// ./csim 256 4 16 write-allocate write-back lru < sometracefile

int main(int argc, char** argv) {
    //Handle case of invalid number of parameters
    if(argc != 7){
        cerr << "Invalid parameters" << endl;
        return 1;
    }
    //Read the command line arguments
    int numSets = stoi(argv[1]);
    int numBlocks = stoi(argv[2]);
    int bytesOfMemory = stoi(argv[3]);
    string type_write_miss = argv[4];
    string type_write_hit = argv[5];
    string replace_strategy = argv[6];

    //create a cache according to the inputs
    Cache cache = create_cache(numSets, numBlocks);

    //Read the trace file and update cache
    string line;
    uint32_t simulation_timestep = 0;
    while(getline(cin, line)){
        stringstream ss(line);
        string command;
        string address;
        ss >> command >> address;


        //Increment simulation timestep
        simulation_timestep++;
        //Look for the element inside the cache
        tuple<int32_t, int32_t> result = find(cache, get_index(stoi(address), numSets, bytesOfMemory), get_tag(stoi(address), numSets, bytesOfMemory));
        //Loading
        if(get<0>(result) == -1){ //Element was not found
            if(replace_strategy == "lru"){
                handle_load_miss_LRU();
            } else if (replace_strategy == "fifo") {
                handle_load_miss_FIFO();
            } else {
                cerr << "Invalid load strategy" << endl;
                return 1;
            }
        } else { //Element was found
            handle_load_hit(cache, get<0>(result), get<1>(result), numBlocks);
        }
        
        
    }

    //Testing
    cout << "numSets: " << numSets << endl << "numBlocks: " << numBlocks  << endl << "bytesOfMemory " << bytesOfMemory << endl;
    cout << type_write_miss << endl;
    cout << type_write_hit << endl;
    cout << replace_strategy << endl;
    cout << "cache size: " << cache.sets.size() << "sets" << endl;
    cout << "set size: " << ((cache.sets)[0]).slots.size() << "slots" << endl;
    cout << get_tag(0x1fffff50, 256, 256) << endl;
    cout << get_index(0x1fffff50, 256, 256) << endl;
    return 0;
}

void handle_load_hit(Cache& cache, uint32_t indexSet, uint32_t indexSlot, uint32_t simulation_timestep){
    cache.sets[indexSet].slots[indexSlot].access_ts = simulation_timestep;
}

void handle_load_miss_LRU(Cache& cache, uint32_t indexSet, uint32_t numBlocks, Slot newSlot){
    uint32_t min = UINT32_MAX;
    int index_LRU = 0;
    //Look for the least recently used slot and replace it with the new slot
    for(int i = 0; i < numBlocks; i++){
        if(cache.sets[indexSet].slots[i].access_ts < min){
            min = cache.sets[indexSet].slots[i].access_ts;
            index_LRU = i;
        }
    }
    cache.sets[indexSet].slots[index_LRU] = newSlot;
}

void handle_load_miss_FIFO(Cache& cache, uint32_t indexSet, uint32_t numBlocks, Slot newSlot){
    uint32_t min = UINT32_MAX;
    int index_FIFO = 0;
    //Look for the earliest loaded slot and replace it with the new slot
    for(int i = 0; i < numBlocks; i++){
        if(cache.sets[indexSet].slots[i].load_ts < min){
            min = cache.sets[indexSet].slots[i].load_ts;
            index_FIFO = i;
        }
    }
    cache.sets[indexSet].slots[index_FIFO] = newSlot;
}