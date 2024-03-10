#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <tuple>
#include "functions.h"

using std::ifstream;
using std::stoul;
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

    //error handling
    //if(input_error_handling(numSets, numBlocks, bytesOfMemory, type_write_miss, type_write_hit) == 1){
    //    return 1;
    //}

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

        //Increment simulation timestep
        simulation_timestep++;    

        if(command == "l"){ //load
            int status = load(stoul(address, nullptr, 16), cache, simulation_timestep);
            totalLoads++;
            if(status == 1){
                totalLoadHits++;
                totalCycles++;
            } else if (status == -1){
                totalLoadMisses++;
                totalCycles = totalCycles + 100 * bytesOfMemory / 4;
            } else {
                cerr << "Invalid input" << endl;
                return 1;
            }
        } else if (command == "s"){ //store
            int status = store(stoul(address, nullptr, 16), cache, simulation_timestep);
            totalStores++;
            if(status == 1){
                totalStoreHits++;
                totalCycles++;
            } else if (status == -1){
                totalStoreMisses++;
                totalCycles = totalCycles + 100 * bytesOfMemory / 4;
            }
        } else {
            cerr << "Invalid command" << endl;
            return 1;
        }
        
           
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