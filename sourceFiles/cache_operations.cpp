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
// givendata is of size of block of the cache.
void cacheSet::putNewData(string givenTag, string givenData, string policy, string in){
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
                if(dirtyBit[i] == 1) updateMemory(i, in);
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
                if(dirtyBit[i] == 1) updateMemory(i, in);
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

void cacheSet::updateMemory(int lineNumber, string in){
    string t = tag[lineNumber] + in;

    for(int i = blockSize; i >= 2; i /= 2){
        t += "0";
    }

    for(int i = 0; i < blockSize; i++){
        memory[t] = data[lineNumber].substr(2 * i, 2);
        t = _add(t, "0000000000000001");
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
void cache::readManager(string addr){
    // Get the aligned address of the block.
    string bin = hexadecimalToBinary(addr);
    
    string in = bin.substr(64 - (byteOffsetBits + indexBits), indexBits);
    string tag = bin.substr(0, tagBits); 

    string alignedAddress = tag + in;
    for(int i = 0; i < byteOffsetBits; i++) alignedAddress += "0";
    alignedAddress = binaryToHexadecimal(alignedAddress);

    // If the cache is not fully associative and the given index of set is present.
    if(in != "" && cacheMem.find(in) != cacheMem.end()){

        // if it is a hit.
        int lineIndex = cacheMem[in].isPresent(tag);
        if(lineIndex != -1){
            hits += 1;
        }

        // If the required tag is not present in the cache set.
        // fetch the required data from memory and put it in the cache set.
        else if(lineIndex == -1){
            misses += 1;

            string requiredData;
            for(int i = 0; i < blockSize; i++){
                requiredData += (memory[alignedAddress] == "") ? "00" : memory[alignedAddress];
                alignedAddress = _add(alignedAddress, "0000000000000001");
            }

            cacheMem[in].putNewData(tag, requiredData, replacePolicy, in);
        }
    }

    // If the cache is not fully associative and the given index of set is not present.
    else if(in != ""){
        // Create the cache set.
        // and put the required data in the cache set.
        cacheMem[in] = cacheSet(associativity, blockSize);

        misses += 1;

        string requiredData;
        for(int i = 0; i < blockSize; i++){
            requiredData += (memory[alignedAddress] == "") ? "00" : memory[alignedAddress];
            alignedAddress = _add(alignedAddress, "0000000000000001");
        }

        cacheMem[in].putNewData(tag, requiredData, replacePolicy, in);
    }

    // If it is fully assoicative.
    else{
        if(cacheMem.find("fullAssociative") != cacheMem.end()){
            int lineIndex = cacheMem["fullAssociative"].isPresent(tag);
            
            if(lineIndex != -1){
                hits += 1;
            }

            else {
                misses += 1;

                string requiredData;
                for(int i = 0; i < blockSize; i++){
                    requiredData += (memory[alignedAddress] == "") ? "00" : memory[alignedAddress];
                    alignedAddress = _add(alignedAddress, "0000000000000001");
                }

                cacheMem[in].putNewData(tag, requiredData, replacePolicy, in);
            }
        }

        else{
            cacheMem["fullAssociative"] = cacheSet(associativity, blockSize);

            misses += 1;

            string requiredData;
            for(int i = 0; i < blockSize; i++){
                requiredData += (memory[alignedAddress] == "") ? "00" : memory[alignedAddress];
                alignedAddress = _add(alignedAddress, "0000000000000001");
            }

            cacheMem[in].putNewData(tag, requiredData, replacePolicy, in);
        }
    }
}

// 16 hexadecimal bits addr. givenData is the modified block data.
void cache::writeManager(string addr, string givenData) {
    string bin = hexadecimalToBinary(addr);

    string in = bin.substr(64 - (byteOffsetBits + indexBits), indexBits);
    string tag = bin.substr(0, tagBits);

    // If the cache is not fully associative and the index is present
    if(in != "" && cacheMem.find(in) != cacheMem.end()){
        int lineIndex = cacheMem[in].isPresent(tag);

        //if it is a hit
        if(lineIndex != -1){
            hits += 1;
            cacheMem[in].updateData(givenData, lineIndex);
        }

        // If the write policy is write through then no need to put the required data in cache.
        // If the write policy is write back fetch the new block to cache.
        else{
            misses += 1;

            if(writePolicy == "WB"){
                cacheMem[in].putNewData(tag, givenData, replacePolicy, in);
            }
        }
    }

    // If the cache is not fully associative and the index is not present
    else if(in != "" && cacheMem.find(in) == cacheMem.end()){
        misses += 1;
        // create the cache set. and if the it is a WB, put the block in cache.
        cacheMem[in] = cacheSet(associativity, blockSize);

        if(writePolicy == "WB")
            cacheMem[in].putNewData(tag, givenData, replacePolicy, in);
    }

    // If the cache is fully associative
    else{
        if(cacheMem.find("fullAssociative") != cacheMem.end()){
            int lineIndex = cacheMem[in].isPresent(tag);
            if(lineIndex != -1){
                hits += 1;
                cacheMem[in].updateData(givenData, lineIndex);
            }

            else {
                misses += 1;

                if(writePolicy == "WB")
                    cacheMem[in].putNewData(tag, givenData, replacePolicy, in);
            }
        }
        else{
            misses += 1;

            cacheMem["fullAssociative"] = cacheSet(associativity, blockSize);

            if(writePolicy == "WB")
                cacheMem[in].putNewData(tag, givenData, replacePolicy, in);
        }
    }
}

// effective Address is the address where a memory operation is performed.
// operation tells the data manager what kind of operation is performed.

string cache::givenDataManager(string addr, string newData, int size) {
    string alignedAddress = hexadecimalToBinary(addr);

    string in = alignedAddress.substr(64 - (byteOffsetBits + indexBits), indexBits);
    string tag = alignedAddress.substr(0, tagBits);
    string byteOffset = alignedAddress.substr(64 - byteOffsetBits, byteOffsetBits);

    alignedAddress = tag + in;

    for(int i = 0; i < byteOffsetBits; i++) alignedAddress += "0";
    alignedAddress = binaryToHexadecimal(alignedAddress);

    string temp = alignedAddress.substr(64 - byteOffsetBits, byteOffsetBits);
    
    string requiredData = "";
    int keep = 0;
    for(int i = 0; i < blockSize; i++){
        if(_isGreaterOrEqualUn(addr, alignedAddress) && keep != size){
            requiredData += newData.substr(2 * keep, 2);
            keep++;
        }
        else{
            requiredData = (memory[alignedAddress] == "") ? "00" : memory[alignedAddress];
        }
        alignedAddress = _add(alignedAddress, "0000000000000001");
    }

    return requiredData;
}

void cache::dumpData(ofstream& outFile){
    // Iterate over all entrues of the cache and print any valid in them.
    auto it = cacheMem.begin();

    while (it != cacheMem.end()){
        it->second.validDataOutput(outFile, it->first);
        it++;
    }
    
}

void cacheSet::validDataOutput(ofstream& outFile, string in){
    for(int i = 0; i < lines; i++){
        if(validBit[i] == 0){
            outFile << "Set: " << in << ", " << "Tag: " << tag[i] << ", ";
            outFile << ((dirtyBit[i] == 0) ? "Clean" : "Dirty") << '\n';
        }
    }
}

void cache::printCacheStats(){
    cout << "D-cache statistics: " << "Accesses=" << (hits + misses) << ", ";
    cout << "Hit=" << hits << ", " << "Miss=" << misses << ", ";
    cout << "Hit Rate=" << float(hits / (hits + misses)) << '\n';
}