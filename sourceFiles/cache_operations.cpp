#include "cache_prototypes.hh"

extern map<string, string> memory;
string cacheState = "disabled";

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
void cacheSet::putNewData(string givenTag, string givenData, string policy, string in, char rw){
    // If there are any invalid line we will use them.
    for(int i = 0; i < this->lines; i++){
        if(validBit[i] == 1){
            tag[i] = givenTag;
            validBit[i] = 0;
            dirtyBit[i] = ((rw == 'w') ? 1 : 0);
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
                dirtyBit[i] = ((rw == 'w') ? 1 : 0);
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
                dirtyBit[i] = ((rw == 'w') ? 1 : 0);
                updateAccess(i);
                updateIndex(i);
                return ;
            }
        }
    }
    // If Random policy
    else if(policy == "RANDOM"){
        random_device seedGenerator;
        mt19937 gen(seedGenerator());

        uniform_int_distribution<int> randLine(0, lines - 1);

        int replaceLine = randLine(gen);

        if(dirtyBit[replaceLine] == 1) updateMemory(replaceLine, in);
        tag[replaceLine] = givenTag;
        data[replaceLine] = givenData;
        dirtyBit[replaceLine] = ((rw == 'w') ? 1 : 0);
        updateAccess(replaceLine);
        updateAccess(replaceLine);
        return; 
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

bool cacheSet::isClean(string t){
    for(int i = 0; i < lines; i++){
        if(tag[i] == t) return dirtyBit[i];
    }
    return false;
}

void cacheSet::invalidate(string in){
    string alignedAddress;
    for(int i = 0; i < lines; i++){
        if(dirtyBit[i] == 1){
            alignedAddress = alignedAddress + in;
            for(int j = blockSize; j >= 2; j /= 2) alignedAddress += "0";

            for(int j = 0; j < blockSize; j++){
                memory[alignedAddress] = data[i].substr(2 * j, 2);
                alignedAddress = _add(alignedAddress, "0000000000000001");
            }
        }
        validBit[i] = 1;
    }
}

cache::cache(int cSize, int bSize, string rpolicy, string wpolicy, int associativity){
    hits = 0;
    misses = 0;
    this->blockSize = bSize;
    indexBits = 0;
    byteOffsetBits = 0;

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

void cache::readManager(string addr, ofstream& outFile){
    string bin = hexadecimalToBinary(addr);
    
    string tag = bin.substr(0, tagBits); 
    string in = bin.substr(tagBits, indexBits);

    string alignedAddress = tag + in;
    for(int i = 0; i < byteOffsetBits; i++) alignedAddress += "0";
    alignedAddress = binaryToHexadecimal(alignedAddress);

    // If the cache is not fully associative and the given index of set is present.
    if(in != "" && cacheMem.find(in) != cacheMem.end()){

        // if it is a hit.
        int lineIndex = cacheMem[in].isPresent(tag);
        if(lineIndex != -1){
            hits += 1;

            outFilePrint('R', addr, in, "Hit", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
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

            cacheMem[in].putNewData(tag, requiredData, replacePolicy, in, 'r');

            outFilePrint('R', addr, in, "Miss", tag, "Clean", outFile);
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

        cacheMem[in].putNewData(tag, requiredData, replacePolicy, in, 'r');

        outFilePrint('R', addr, in, "Miss", tag, "Clean", outFile);
    }

    // If it is fully assoicative.
    else{
        if(cacheMem.find("fullAssociative") != cacheMem.end()){
            int lineIndex = cacheMem["fullAssociative"].isPresent(tag);
            
            if(lineIndex != -1){
                hits += 1;
            outFilePrint('R', addr, in, "Hit", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);

            }

            else {
                misses += 1;

                string requiredData;
                for(int i = 0; i < blockSize; i++){
                    requiredData += (memory[alignedAddress] == "") ? "00" : memory[alignedAddress];
                    alignedAddress = _add(alignedAddress, "0000000000000001");
                }

                cacheMem[in].putNewData(tag, requiredData, replacePolicy, in, 'r');

                outFilePrint('R', addr, in, "Miss", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
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

            cacheMem[in].putNewData(tag, requiredData, replacePolicy, in, 'r');

            outFilePrint('R', addr, in, "Miss", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
        }
    }
}

void cache::writeManager(string addr, string givenData, ofstream& outFile) {
    string bin = hexadecimalToBinary(addr);

    string tag = bin.substr(0, tagBits);
    string in = bin.substr(tagBits, indexBits);

    // If the cache is not fully associative and the index is present
    if(in != "" && cacheMem.find(in) != cacheMem.end()){
        int lineIndex = cacheMem[in].isPresent(tag);

        //if it is a hit
        if(lineIndex != -1){
            hits += 1;
            cacheMem[in].updateData(givenData, lineIndex);
            
            outFilePrint('W', addr, in, "Hit", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
        }

        // If the write policy is write through then no need to put the required data in cache.
        // If the write policy is write back fetch the new block to cache.
        else{
            misses += 1;

            if(writePolicy == "WB"){
                cacheMem[in].putNewData(tag, givenData, replacePolicy, in, 'w');
            }

            outFilePrint('W', addr, in, "Miss", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
        }
    }

    // If the cache is not fully associative and the index is not present
    else if(in != "" && cacheMem.find(in) == cacheMem.end()){
        misses += 1;
        // create the cache set. and if the it is a WB, put the block in cache.
        cacheMem[in] = cacheSet(associativity, blockSize);

        if(writePolicy == "WB")
            cacheMem[in].putNewData(tag, givenData, replacePolicy, in, 'w');

        outFilePrint('W', addr, in, "Miss", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
    }

    // If the cache is fully associative
    else{
        if(cacheMem.find("fullAssociative") != cacheMem.end()){
            int lineIndex = cacheMem[in].isPresent(tag);
            if(lineIndex != -1){
                hits += 1;
                cacheMem[in].updateData(givenData, lineIndex);
                outFilePrint('W', addr, in, "Hit", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);

            }

            else {
                misses += 1;

                if(writePolicy == "WB")
                    cacheMem[in].putNewData(tag, givenData, replacePolicy, in, 'w');

                outFilePrint('W', addr, in, "Miss", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
            }
        }
        else{
            misses += 1;

            cacheMem["fullAssociative"] = cacheSet(associativity, blockSize);

            if(writePolicy == "WB")
                cacheMem[in].putNewData(tag, givenData, replacePolicy, in, 'w');

            outFilePrint('W', addr, in, "Miss", tag, ((cacheMem[in].isClean(tag) == 0) ? "Clean" : "Dirty"), outFile);
        }
    }
}

string cache::givenDataManager(string addr, string newData, int size) {
    string alignedAddress = hexadecimalToBinary(addr);

    string tag = alignedAddress.substr(0, tagBits);
    string in = alignedAddress.substr(tagBits, indexBits);
    string byteOffset = alignedAddress.substr(64 - byteOffsetBits, byteOffsetBits);

    alignedAddress = tag + in;

    for(int i = 0; i < byteOffsetBits; i++) alignedAddress += "0";

    string temp = alignedAddress.substr(64 - byteOffsetBits, byteOffsetBits);
    
    alignedAddress = binaryToHexadecimal(alignedAddress);
    string requiredData = "";
    int keep = 0;
    for(int i = 0; i < blockSize; i++){
        if(_isGreaterOrEqualUn(addr, alignedAddress) && keep < size){
            requiredData += newData.substr(2 * keep, 2);
            keep++;
        }
        else{
            requiredData += (memory[alignedAddress] == "") ? "00" : memory[alignedAddress];
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

void cache::clearCache(){
    cacheMem.clear();
    hits = 0;
    misses = 0;
}

void cache::cacheInvalidate(){
    auto it = cacheMem.begin();

    while(it != cacheMem.end()){
        it->second.invalidate(it->first);
        it++;
    }
}

void cacheSet::validDataOutput(ofstream& outFile, string in){
    for(int i = 0; i < lines; i++){
        if(validBit[i] == 0){
            outFile << "Set: " << pretty(inHex(in)) << ", " << "Tag: " << pretty(inHex(tag[i])) << ", ";
            outFile << ((dirtyBit[i] == 0) ? "Clean" : "Dirty") << '\n';
        }
    }
}

void cache::printCacheStats(){
    cout << "D-cache statistics: " << "Accesses=" << (hits + misses) << ", ";
    cout << "Hit=" << hits << ", " << "Miss=" << misses << ", ";
    cout << "Hit Rate=" << setprecision(3) << float(hits) / (hits + misses) << '\n';
}

string inHex(string s){
    int l = s.size();

    if(l <= 64){
        for(int i = 0; i < (64 - l); i++){
            s = "0" + s;
        }
    }

    return binaryToHexadecimal(s);
}

void outFilePrint(char rw, string addr, string in, string hm, string tag, string dc, ofstream& outFile){
    outFile << rw << ": " << "Address: " << pretty(addr) << ", " << "Set: " << pretty(inHex(in)) << ", ";
    outFile << hm << ", " << "Tag: " << pretty(inHex(tag)) << ", " << dc << '\n';
}

string pretty(string s){
    int index = 0;

    for(index = 0; index < s.size(); index++){
        if(s[index] != '0') break;
    }

    if(index == s.size()) index -= 1;
    return "0x" + s.substr(index);
}