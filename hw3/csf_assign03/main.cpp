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
    Cache cache = create_cache(numSets, numBlocks, bytesOfMemory, replace_strategy, type_write_miss, type_write_hit);

    //Counter variables to display later
    uint32_t totalLoads = 0;
    uint32_t totalStores = 0;
    uint32_t totalLoadHits = 0;
    uint32_t totalLoadMisses = 0;
    uint32_t totalStoreHits = 0;
    uint32_t totalStoreMisses = 0;
    uint32_t totalCycles = 0;

    //Read the trace file and update cache
    string line;
    uint32_t simulation_timestep = 0;
    while(getline(cin, line)){
        //Read in line from input file and store the command and address
        stringstream ss(line);
        string command;
        string address;
        ss >> command >> address;

        //Look for the element inside the cache
        uint32_t index = get_index(stoi(address), numSets, bytesOfMemory);
        uint32_t tag = get_tag(stoi(address), numSets, bytesOfMemory);
        tuple<int32_t, int32_t> result = find(cache, index, tag);

        if(command == "l"){ //load
            int status = load(stoi(address), cache, simulation_timestep);
            totalLoads++;
            if(status == 1){
                totalLoadHits++;
            } else if (status == -1){
                totalLoadMisses++;
            } else {
                cerr << "Invalid input" << endl;
                return 1;
            }
        } else if (command == "s"){ //store
            store(stoi(address), cache, simulation_timestep);
            totalStores++;
        } else {
            cerr << "Invalid command" << endl;
            return 1;
        }

        //Increment simulation timestep
        simulation_timestep++;       
    }

    //Testing
    //cout << "numSets: " << numSets << endl << "numBlocks: " << numBlocks  << endl << "bytesOfMemory " << bytesOfMemory << endl;
    //cout << type_write_miss << endl;
    //cout << type_write_hit << endl;
    //cout << replace_strategy << endl;
    //cout << "cache size: " << cache.sets.size() << "sets" << endl;
    //cout << "set size: " << ((cache.sets)[0]).slots.size() << "slots" << endl;
    //cout << get_tag(0x1fffff50, 256, 256) << endl;
    //cout << get_index(0x1fffff50, 256, 256) << endl;
    cout << "Total loads: " << totalLoads << endl;
    cout << "Total stores: " << totalStores << endl;
    cout << "Load hits  " << totalLoadHits << endl;
    cout << "Load misses: " << totalLoadMisses << endl;
    cout << "Store hits: " << totalStoreHits << endl;
    cout << "Store misses: " << totalStoreMisses << endl;
    cout << "Total cycles: " << totalCycles << endl;
    
    return 0;
}