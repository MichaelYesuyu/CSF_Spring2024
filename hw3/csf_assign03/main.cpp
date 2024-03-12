#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <tuple>
#include <chrono>
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
    if(input_error_handling(numSets, numBlocks, bytesOfMemory, type_write_miss, type_write_hit) == 1){
        return 1;
    }

    //create a cache according to the inputs
    Cache cache = create_cache(numSets, numBlocks, bytesOfMemory, replace_strategy, type_write_miss, type_write_hit);

    //Read the trace file and update cache
    string line;
    while(getline(cin, line)){
        //Read in line from input file and store the command and address
        stringstream ss(line);
        string command;
        string address;
        ss >> command >> address;

        if(command == "l"){ //load
            load(stoul(address, nullptr, 16), cache);
        } else if (command == "s"){ //store
            store(stoul(address, nullptr, 16), cache);
        } else {
            cerr << "Invalid command" << endl;
            return 1;
        }
        
    }

    cout << "Total loads: " << cache.totalLoads << endl;
    cout << "Total stores: " << cache.totalStores << endl;
    cout << "Load hits: " << cache.totalLoadHits << endl;
    cout << "Load misses: " << cache.totalLoadMisses << endl;
    cout << "Store hits: " << cache.totalStoreHits << endl;
    cout << "Store misses: " << cache.totalStoreMisses << endl;
    cout << "Total cycles: " << cache.totalCycles << endl;
    
    return 0;
}

