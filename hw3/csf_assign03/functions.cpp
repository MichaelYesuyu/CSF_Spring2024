#include "functions.h"

Cache create_cache(int numSets, int numBlocks, int bytesOfMemory){
    Cache cache;
    for (int i = 0; i < numSets; i++){
        Set set;
        for (int j = 0; j < numBlocks; j++){
            Slot slot = new Slot();
            set.slots.push_back(slot);
        }
        cache.sets.push_back(set);
    }
    return cache;
}