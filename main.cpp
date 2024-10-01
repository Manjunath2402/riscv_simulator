#include "./sim_prototypes.hh"

int main(){
    ifstream inputFile;
    // cout << immediateGenerator("2") << endl;
    // cout << hexadecimalToDecimal("234fe") << endl;
    while (true){
        string input;
        int lineNumber = 0;
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
            setBufferFromTextSeg(inputFile);
            setRegistersToZero();
            lineNumber = 0;
        }
        else if(fields[0] == "step"){
            string temp = step(inputFile, lineNumber);
            if(temp == ""){
                cout << "Nothing to step" << endl;
            }
            else{
                cout << "Executed instruction " << temp << "; ";
                getPC();
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
        else if(fields[0] == "exit"){
            cout << "Exiting the simulator." << endl;
            inputFile.close();
            exit(0);
        }
        cout << endl;
    }
    
    return 0;
}
