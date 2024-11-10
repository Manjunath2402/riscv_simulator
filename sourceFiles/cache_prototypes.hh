#include "alu_prototypes.hh"
#include <vector>
#include <string>
#include <map>
using namespace std;

/*
cache is implemented as map from string(index) to set(collection of blocks(implemented as strings)).
A set is a collection of blocks or lines. A block or a cache line will consist of a tag, valid bit,
dirty bit and the size of the block and additional info required for replacement.
*/

class cacheSet{

private:
    int lines; // the associativity of the cache.
    int blockSize; // the block size of the cache in bytes. 

    vector<string> tag; // tag for each line.

    // 0 is assumed to be valid bit and 1 is assumed to be invalid bit.
    vector<bool> validBit; // valid bit for each line.

    // 0 is assumed to be clean bit and 1 is assumed to be dirty bit.
    vector<bool> dirtyBit; // dirty bit for each line.

    vector<string> data; // actual data is stored here.
    
    vector<int> recentAccess; // used for prioritize each cache line based on recent access. 
    // Used for LRU. 0 means latest accessed.

    vector<int> timeIndex; // Used for FIFO replacement.
    // 1 means the block that is been in cache for long time.
    // timeIndex = lines means the latest block in the cache set.
public:
    cacheSet(int associativity, int bsize) {
        blockSize = bsize;
        lines = associativity;

        // for each line one tag.
        tag = vector<string> (associativity, "");

        // by default all lines are invalid. So initialised with 1.
        validBit = vector<bool> (associativity, 1);

        // by default all the lines are clean. So initialised with 0.
        dirtyBit = vector<bool> (associativity, 0);

        // every line in the starting will have 0. So everything is recently accessed.
        recentAccess = vector<int> (associativity, 0);

        // In the start every block will have 0 time index.
        timeIndex = vector<int> (associativity, 0);

        // all the data is initialised with "00".
        string temp = "";
        for(int i = 0; i < bsize; i++){
            temp += "00";
        }

        data = vector<string> (associativity, temp);
    }

    // based on a given address, checks whether the data for the requested addr is present or not
    // and return index if present else -1.
    int isPresent(string requiredTag);

    // updates recent access of a block with given line number.
    void updateAccess(int lineNumber);
    // updates time index of a block with given line number.
    void updateIndex(int lineNumber);

    // default replacement policy is LRU. puts new data in the cache set.
    void putNewData(string givenTag, string givenData, string policy = "LRU");

    // update data in the given cache set line.
    void updateData(string givenData, int lineNumber);

    string getData(int lineNumber) const ;
};

// cache manager prototypes.

class cache{

private:
    map<string, cacheSet> cacheMem;
    string replacePolicy;
    string writePolicy;
    int numberOfSets;
    int associativity;
    int blockSize;

    int tagBits;
    int indexBits;
    int byteOffsetBits;

    int hits;
    int misses;
public:
    cache(int cSize, int bSize, string rpolicy, string wpolicy, int associativity);
    void readManager(string addr, int offset, int size);
    void writeManager(string addr, string givenData, int offset, int size);
};