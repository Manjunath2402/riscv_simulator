#include "./sim_prototypes.hh"

extern string cacheState;
extern cache myCache;
ofstream outputFile;

int main(){
    ifstream inputFile;

    ifstream configFile;
    ofstream dumpFile;
    string cacheConfigInfo[5];
    string filename = "";
    string fields[4] = {"", "", "", ""};
    string input;

    int lineNumber;
    while (true){
        
        getline(cin, input);
        input = lineParser(input);

        int fieldCount = 0;
        int index = 0;
        while (true){
            if(input[index] != ' ' && input[index] != '\0') fields[3] += input[index];
            else{
                fields[fieldCount] = fields[3];
                fieldCount += 1;
                fields[3] = "";
            }
            if(input[index] == '\0') break;
            index++;
        }
        
        if(fields[0] == "load"){
            outputFile.close();
            inputFile.close();
            inputFile.open(fields[1], ios::in);
            filename = fields[1];
            // to clear all things set from previous load.
            setMemoryToZero();
            setRegistersToZero();
            clearBreakPoint();

            // these are for new loaded file.
            createCallStack();
            labelParser(inputFile);
            initialiseDataSegment(inputFile);
            setBufferFromTextSeg(inputFile, lineNumber);

            if(cacheState == "enabled"){
                myCache.clearCache();

                int i = filename.size();
            
                for(; i >= 0; i--){
                    if(filename[i] == '.') break;
                }

                string temp = filename.substr(0, i + 1);
                temp = temp + "output";

                outputFile.open(temp, ios::out);
            }
        }
        else if(fields[0] == "step"){
            if(step(inputFile, lineNumber) == ""){
                cout << "Nothing to step" << endl;
            }
        }
        else if(fields[0] == "run"){
            run(inputFile, lineNumber);
        }
        else if(fields[0] == "regs"){
            printRegisterValues();
        }
        else if(fields[0] == "mem"){
            int bytes = stoi(fields[2]);
            printMemory(fields[1], bytes);
        }
        else if(fields[0] == "break"){
            setBreakPoint(stoi(fields[1]));
        }
        else if(fields[0] == "del"){
            removeBreakPoint(stoi(fields[2]));
        }
        else if(fields[0] == "show-stack"){
            printCallStack();
        }
        else if(fields[0] == "exit"){
            cout << "Exited the simulator." << endl;
            inputFile.close();
            outputFile.close();
            exit(0);
        }
        else if(fields[0] == "cache_sim" && fields[1] == "enable"){
            configFile.open(fields[2], ios::in);

            for(int i = 0; i < 5; i++){
                getline(configFile, cacheConfigInfo[i]);
                cacheConfigInfo[i] = lineParser(cacheConfigInfo[i]);
            }

            int cacheSize = stoi(cacheConfigInfo[0]);
            int blockSize = stoi(cacheConfigInfo[1]);
            int associativity = stoi(cacheConfigInfo[2]);

            myCache = cache(cacheSize, blockSize, cacheConfigInfo[3], cacheConfigInfo[4], associativity);

            cacheState = "enabled";

            configFile.close();
        }

        else if(fields[0] == "cache_sim" && fields[1] == "disable"){
            cacheState = "disabled";
        }

        else if(fields[0] == "cache_sim" && fields[1] == "status"){
            //check whether cache simulation has been enabled
            if(cacheState == "disabled"){
                cout << "Cache simulation is disabled." << endl;
            }

            else{
                cout << "Cache Size: "<<cacheConfigInfo[0] << endl;
                cout << "Block Size: "<<cacheConfigInfo[1] << endl;
                cout << "Associativity: "<<cacheConfigInfo[2] << endl;
                cout << "Replacement Policy: "<<cacheConfigInfo[3] << endl;
                cout << "Write Back Policy: "<<cacheConfigInfo[4] << endl;
            }
        }

        else if(fields[0] == "cache_sim" && fields[1] == "invalidate"){
            if(cacheState == "disabled"){
                cout << "Cache simulation is disabled." << endl;
            }
            else{
                myCache.cacheInvalidate();
            }
        }

        else if(fields[0] == "cache_sim" && fields[1] == "dump"){
            if(cacheState == "enabled"){
                dumpFile.open(fields[2], ios::out);
                myCache.dumpData(dumpFile);
                dumpFile.close();
            }
            else{
                cout << "Cache simulation is disabled." << endl;
            }
        }

        else if(fields[0] == "cache_sim" && fields[1] == "stats"){
            if(cacheState == "enabled")
                myCache.printCacheStats();
            
            else
                cout << "Cache Simulator is not enabled." << endl;
        }
        
        cout << endl;
    }
    
    return 0;
}