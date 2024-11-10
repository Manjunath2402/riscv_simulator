#include "cache_prototypes.hh"
extern map<string, string> memory;

// given a tag return if it is present in the cache or not.
// If present update the recent access data.
int cacheSet::isPresent(string requiredTag){
    for(int i = 0; i < this->lines; i++){
        if(requiredTag == tag[i] && validBit[i] == 0){
            updateAccess(i);
            return i;
        }
    }

    return -1;
}

inline string cacheSet::getData(int lineNumber) const {
    return data[lineNumber];
}

void cacheSet::updateAccess(int lineNumber){
    // update the lines which are recently accessed than given line.
    for(int i = 0; i < this->lines; i++){
        if(recentAccess[lineNumber] > recentAccess[i] && validBit[i] == 0){
            recentAccess[i] += 1;
        }
    }
    recentAccess[lineNumber] = 0;
}

void cacheSet::updateIndex(int lineNumber){
    static int i = 0;
    if(i < lines){
        i++;
        timeIndex[lineNumber] = i;
        return ;
    }
    
    timeIndex[lineNumber] = lines;
    for(int j = 0; j < lines; j++){
        if(j != lineNumber) timeIndex[j] -= 1;
    }
}

// Based on the policy update the cache set with given data and given tag.
// 0 is for LRU, 1 is FIFO and 2 is Random.
void cacheSet::putNewData(string givenTag, string givenData, string policy = "LRU"){
    // If there are any invalid line we will use them.
    for(int i = 0; i < this->lines; i++){
        if(validBit[i] == 1){
            tag[i] = givenTag;
            validBit[i] = 0;
            data[i] = givenData;
            updateAccess(i);
            updateIndex(i);
            return ;
        }
    }

    // If there is no invalid line. Based on the policy we will replace a line.
    // If LRU policy
    if(policy == "LRU"){
        // recent access with number of lines - 1 should be replaced.
        for(int i = 0; i < lines; i++){
            if(recentAccess[i] == lines - 1){
                tag[i] = givenTag;
                data[i] = givenData;
                updateAccess(i);
                updateIndex(i);
                return ;
            }
        }
    }
    // If FIFO policy
    else if(policy == "FIFO"){
        // replace the block with time index 1. which is been in cache for long time.
        for(int i = 0; i < lines; i++){
            if(timeIndex[i] == 1){
                tag[i] = givenTag;
                data[i] = givenData;
                updateAccess(i);
                updateIndex(i);
                return ;
            }
        }
    }
    // If Random policy
    else if(policy == "RANDOM"){

    }
}

void cacheSet::updateData(string givenData, int lineNumber){
    data[lineNumber] = givenData;
    dirtyBit[lineNumber] = 1;
    updateAccess(lineNumber);
    updateIndex(lineNumber);
}

// We are assuming 64 bit addressing.
cache::cache(int cSize, int bSize, string rpolicy, string wpolicy, int associativity){
    hits = 0;
    misses = 0;
    this->blockSize = bSize;

    replacePolicy = rpolicy;
    writePolicy = wpolicy;

    if(associativity == 0) {
        numberOfSets = 1;
        associativity = cSize / bSize;
    }
    else{
        numberOfSets = (cSize / (bSize * associativity));
    }
    this->associativity = associativity;

    while (numberOfSets >= 2){
        numberOfSets /= 2;
        indexBits += 1;
    }
    
    while (bSize >= 2){
        bSize /= 2;
        byteOffsetBits += 1;
    }

    tagBits = 64 - (indexBits + byteOffsetBits);
}

// 16 hexadecimal bits addr, size is in bytes.
void cache::readManager(string addr, int offset, int size){
    string bin = hexadecimalToBinary(addr);
    
    // Index can be an empty string in case of fully associative cache.
    string in = bin.substr(64 - (byteOffsetBits + indexBits), indexBits);
    string tag = bin.substr(0, tagBits);

    // If the cache is not fully associative and the index is present.
    if(in != "" && cacheMem.find(in) != cacheMem.end()){
        // if it is a hit.
        int lineIndex = cacheMem[in].isPresent(tag);
        if(lineIndex != -1){
            hits += 1;
        }

        // If the required data is not present in the cache set.
        else if(lineIndex == -1){
            misses += 1;

            string temp = tag + in;
            for(int i = 0; i < byteOffsetBits; i++) temp += "0";
            temp = binaryToHexadecimal(temp);

            string requiredData;
            for(int i = 0; i < blockSize; i++){
                requiredData += (memory[temp] == "") ? "00" : memory[temp];
                temp = _add(temp, "0000000000000001");
            }

            cacheMem[in].putNewData(tag, requiredData, replacePolicy);
        }
    }

    // If the cache is not fully associative and if index is not present.
    else if(in != ""){
        cacheMem[in] = cacheSet(associativity, blockSize);

        misses += 1;

        string temp = tag + in;
        for(int i = 0; i < byteOffsetBits; i++) temp += "0";
        temp = binaryToHexadecimal(temp);

        string requiredData;
        for(int i = 0; i < blockSize; i++){
            requiredData += (memory[temp] == "") ? "00" : memory[temp];
            temp = _add(temp, "0000000000000001");
        }

        cacheMem[in].putNewData(tag, requiredData, replacePolicy);
    }

    // If it is fully assoicative.
    else{
        // There will be only one set.
        // We will name this set as full Associative set.
        if(cacheMem.find("fullAssociative") != cacheMem.end()){
            int lineIndex = cacheMem["fullAssociative"].isPresent(tag);
            if(lineIndex != -1){
                hits += 1;
            }

            else {
                misses += 1;

                string temp = tag + in;
                for(int i = 0; i < byteOffsetBits; i++) temp += "0";
                temp = binaryToHexadecimal(temp);

                string requiredData;
                for(int i = 0; i < blockSize; i++){
                    requiredData += (memory[temp] == "") ? "00" : memory[temp];
                    temp = _add(temp, "0000000000000001");
                }

                cacheMem[in].putNewData(tag, requiredData, replacePolicy);
            }
        }

        else{
            cacheMem["fullAssociative"] = cacheSet(associativity, blockSize);

            misses += 1;

            string temp = tag + in;
            for(int i = 0; i < byteOffsetBits; i++) temp += "0";
            temp = binaryToHexadecimal(temp);

            string requiredData;
            for(int i = 0; i < blockSize; i++){
                requiredData += (memory[temp] == "") ? "00" : memory[temp];
                temp = _add(temp, "0000000000000001");
            }

            cacheMem[in].putNewData(tag, requiredData, replacePolicy);
        }
    }
}

