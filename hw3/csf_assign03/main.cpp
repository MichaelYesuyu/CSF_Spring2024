#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
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
    Cache cache = create_cache(numSets, numBlocks, bytesOfMemory);

    //Read the trace file and update cache
    string line;
    while(getline(cin, line)){
        stringstream ss(line);
        string command;
        string address;
        ss >> command >> address;
        cout << command << " " << address << endl;
        //Call some function to handle
        //updateCache(command, address, type_write_miss, type_write_hit, replace_strategy);
    }

    //Testing
    cout << "numSets: " << numSets << endl << "numBlocks: " << numBlocks  << endl << "bytesOfMemory " << bytesOfMemory << endl;
    cout << type_write_miss << endl;
    cout << type_write_hit << endl;
    cout << replace_strategy << endl;
    return 0;
}