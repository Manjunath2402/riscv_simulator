#include "./sim_prototypes.hh"

int main(){
    ifstream inputFile;
    // cout << immediateGenerator("2") << endl;
    // cout << hexadecimalToDecimal("234fe") << endl;
    int lineNumber;
    while (true){
        string input;
        
        getline(cin, input);
        input = lineParser(input);

        string fields[4] = {"", "", "", ""};
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
            inputFile.close();
            inputFile.open(fields[1], ios::in);

            labelParser(inputFile);
            initialiseDataSegment(inputFile);
            setBufferFromTextSeg(inputFile, lineNumber);
            setRegistersToZero();
            clearBreakPoint();
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
            
        }
        else if(fields[0] == "exit"){
            cout << "Exiting the simulator." << endl;
            inputFile.close();
            exit(0);
        }
        cout << endl;
    }
    
    return 0;
}
