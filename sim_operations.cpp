#include "./sim_prototypes.hh"

extern map<int, char> decToHex;

map<string, int> regNameToRegNum = {

    {"x0", 0}, {"x1", 1}, {"x2", 2}, {"x3", 3}, {"x4", 4}, {"x5", 5},
    {"x6", 6}, {"x7", 7}, {"x8", 8}, {"x9", 9}, {"x10", 10}, {"x11", 11},
    {"x12", 12}, {"x13", 13}, {"x14", 14}, {"x15", 15}, {"x16", 16}, {"x17", 17},
    {"x18", 18}, {"x19", 19}, {"x20", 20}, {"x21", 21}, {"x22", 22}, {"x23", 23},
    {"x24", 24}, {"x25", 25}, {"x26", 26}, {"x27", 27}, {"x28", 28}, {"x29", 29},
    {"x30", 30}, {"x31", 31},

    {"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5},
    {"t1", 6}, {"t2", 7}, {"s0", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11},
    {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17},
    {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
    {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}, {"t3", 28}, {"t4", 29},
    {"t5", 30}, {"t6", 31},

    {"fp", 8}, {"pc", 32}
};

map<int, string> regNumToRegValue = {
    {0, "0000000000000000"}, {1, "0000000000000000"}, {2, "0000000000000000"}, {3, "0000000000000000"},
    {4, "0000000000000000"}, {5, "0000000000000000"}, {6, "0000000000000000"}, {7, "0000000000000000"}, 
    {8, "0000000000000000"}, {9, "0000000000000000"}, {10, "0000000000000000"}, {11, "0000000000000000"}, 
    {12, "0000000000000000"}, {13, "0000000000000000"}, {14, "0000000000000000"}, {15, "0000000000000000"}, 
    {16, "0000000000000000"}, {17, "0000000000000000"}, {18, "0000000000000000"}, {19, "0000000000000000"}, 
    {20, "0000000000000000"}, {21, "0000000000000000"}, {22, "0000000000000000"}, {23, "0000000000000000"}, 
    {24, "0000000000000000"}, {25, "0000000000000000"}, {26, "0000000000000000"}, {27, "0000000000000000"}, 
    {28, "0000000000000000"}, {29, "0000000000000000"}, {30, "0000000000000000"}, {31, "0000000000000000"},

    {32, "0000000000000000"} // This is PC register.
};

map<string, int> labelData;

map<string, string> memory;
set<int> breakPoints;


map<string, function<string (string, string)>> RFunctionMap = {
    {"add", _add}, {"sub", _sub}, {"xor", _xor}, {"or", _or}, {"and", _and}, {"sll", _sll},
    {"srl", _srl}, {"sra", _sra}, {"slt", _slt}, {"sltu", _sltu}
};

map<string, function<string (string, string)>> IFunctionMap = {
    {"addi", _add}, {"xori", _xor}, {"ori", _or}, {"andi", _and}, {"slli", _sll},
    {"srli", _srl}, {"srai", _sra}, {"slti", _slt}, {"sltiu", _sltu}, {"jalr", _add}
};

map<string, function<bool (string, string)>> BFunctionMap = {
    {"beq", _isEqual}, {"bne", _isNotEqual}, {"bge", _isGreaterOrEqual},
    {"blt", _isLessThan}, {"bltu", _isLessThanUn}, {"bgeu", _isGreaterOrEqualUn}
};
// A map of R format operations and this function pointers.

set<string> RFormatInstructions = {"add", "sub", "xor", "or", "and", "sll", "srl", "sra", "slt", "sltu"};
set<string> IFormatInstructions1 = {"addi", "xori", "ori", "andi", "slli", "srli", "srai", "slti", "sltiu",
    "jalr" };
set<string> IFormatInstructions2 = {
    "lb", "lw", "lh", "ld", "lbu", "lwu", "lhu"
};
set<string> SFormatInstructions = {"sd", "sw", "sh", "sb"};
set<string> BFormatInstructions = {"beq", "bne", "blt", "bge", "bgeu", "bltu"};
set<string> JFormatInstructions = {"jal"};
set<string> UFormatInstructions = {"lui", "auipc"};

int startOfTextSeg = 0;

stack<functionStack> callStack;
functionStack functionCalls[100];
int functionCallCount = 0;

void createCallStack(){
    functionCalls[0].updateLabel("main");
    functionCalls[0].updateLine(0);
    callStack.push(functionCalls[0]);
    functionCallCount += 1;
}

void updateCallStack(int line, int flag, string label = ""){
    if(flag == 0){
        callStack.top().updateLine(line);
    }
    else if(flag == 1){
        functionCalls[functionCallCount].updateLabel(label);
        functionCalls[functionCallCount].updateLine(line);
        callStack.push(functionCalls[functionCallCount]);
    }
    else if(flag == 2){
        callStack.pop();
        callStack.top().updateLine(line);
    }
}

void printCallStack(){
    stack<functionStack> copyStack(callStack);
    functionStack temp;
    cout << "Call Stack: " << endl;
    while (!copyStack.empty()){
        cout << copyStack.top().showLabel() << ": " << copyStack.top().showLineNumber() << endl;
        copyStack.pop();
    }
}

string immediateGenerator(string op1){
    string result = decimalToBinary(op1);
    // cout << result << endl;
    reverse(result.begin(), result.end());

    int length = result.size();
    char sign = result[length - 1];
    for (size_t i = 0; i < (64 - length); i++){
        result += sign;
    }
    result = result.substr(0, 64);
    // cout << result << endl;
    reverse(result.begin(), result.end());
    result = binaryToHexadecimal(result);
    // cout << result << endl;
    return result;
}

/**
 * @brief This function is used by immediate generator.
 * @param s a decimal number.
 * @return return the two's complement representation of that number.
 */
string decimalToBinary(string s){
    string bin = "";
    int dec = 0;
    int i = 0;
    int check = -1;

    if(s[i] == '-'){
        i = 1;
    }
    for(; i<s.length(); i++){
        dec = dec*10 + int(s[i] - '0'); 
    }
    if(s[0] == '-'){
        dec = -dec;
        check = 0;
    }

    while(dec != 0){
        if(dec%2==0){
            bin += "0";
        }
        else{
            bin += "1";
        }

        dec/=2;
    }
    while (bin.size() < 64)
    {
        bin += '0';
    }
    
    if(check == 0){
        for(int j = 0; j<bin.size(); j++){    
            if (bin[j] == '0' && check == 0){
                bin[j] = '0';
            }
            else if(bin[j] == '1' && check == 0){
                bin[j] = '1';
                check = 1;
            } 
            else if(bin[j] == '1' && check == 1){
                bin[j] = '0';
            }
            else if(bin[j] == '0' && check == 1){
                bin[j] = '1';
            }
        }
    }
    reverse(bin.begin(), bin.end());
    
    return bin;
}


/**
 * @brief Goes through the file once and gets the label and their corresponding line numbers.
 * @param in a reference to the file stream of the input file
 * @return return void.
 */
void labelParser(ifstream& in){
    in.clear();
    in.seekg(0);

    string s;
    int lineNumber = 0;
    while (true){
        if(in.eof()) break;

        getline(in, s);
        lineNumber++;

        char iterator = s[0];
        int index = 0;
        string label = "";
        string temp = "";

        while (iterator != '\0'){
            if(iterator == ':'){
                label = temp;
                break;
            }
            temp += iterator;
            index++;
            iterator = s[index];
        }

        // if there is no colon in the instruction then label will never be assigned.
        if(label != ""){
            if(labelData[label] == 0){
                labelData[label] = lineNumber;
            }
        }

    }
    in.clear();
    in.seekg(0);
}

void initialiseDataSegment(ifstream& in){
    in.clear();
    in.seekg(0);

    string segmentStart = "0000000000010000";
    string s = "";
    int flag = 0;
    while (true){
        if(in.eof()) break;
        getline(in, s);
        s = lineParser(s);
        if(s == ".data") {flag = 1; continue;}
        else if(s == ".text") break;
        else if(s == "") continue;
        if(flag == 1){
            vector<string> fields(100);
            string temp = "";

            int index = 0;
            int count = 0;
            int spaceFlag = 0;
            while (true){
                if((s[index] == ' ' && spaceFlag == 0) && s[index] != '\0'){
                    count++;
                    spaceFlag = 1;
                }
                else if(s[index] != ',' && s[index] != '\0'){
                    fields[count] += s[index];
                    spaceFlag = 0;
                }

                if(s[index] == '\0') break;
                index++;
            }
            
            int bytes = 8;
            int bits = 16;
            if(fields[0] == ".word") {bytes = 4; bits = 8;}
            if(fields[0] == ".half") {bytes = 2; bits = 4;}
            if(fields[0] == ".byte") {bytes = 1; bits = 2;}
            
            for (int i = 1; i <= count; i++){
                if(fields[i].substr(0, 2) == "0x"){
                    fields[i] = fields[i].substr(2);
                }
                else{
                    fields[i] = decimalToHexadecimal(fields[i]);
                    if(fields[0] == ".word"){
                        fields[i] = fields[i].substr(8);
                    }
                    else if(fields[0] == ".half"){
                        fields[i] = fields[i].substr(12);
                    }
                    else if(fields[0] == ".byte"){
                        fields[i] = fields[i].substr(14);
                    }
                }

                for (int j = bytes - 1; j >= 0; j--){
                    memory[segmentStart] = fields[i].substr(2 * j, 2);
                    segmentStart = RFunctionMap["add"](segmentStart, "0000000000000001");
                }
            }
            
        }
    }
    
    in.clear();
    in.seekg(0);
}

void setBufferFromTextSeg(ifstream& in, int& lineNumber){
    in.clear();
    in.seekg(0);

    lineNumber = 0;
    string s = "";
    do{
        getline(in, s);
        lineNumber++;
        if(s == ".text") break;
    } while (!(in.eof()));
    lineNumber++;

    // If .text segment is not present in the file then it is considered .text segment starts from line number 1.
    if(in.eof()){
        lineNumber = 1;
        in.clear();
        in.seekg(0);
    }
    startOfTextSeg = lineNumber;
}

void executeInstruction(string s, int& lineNumber, ifstream& in){
    char iterator = s[0];
    string temp[5] = {"", "", "", "", ""};

    int i = 0;
    int index = 0;
    int spaceFlag = 0;
    
    while (iterator != '\0'){

        if(iterator == ':'){
            temp[i] = "";
            i = -1;
        }
        else if((iterator == ' ' && spaceFlag == 0) || iterator == '('){
            i++;
            spaceFlag = 1;
        }
        else if(iterator != ',' && iterator != ')' && iterator != ' '){
            temp[i] += iterator;
            spaceFlag = 0;
        }

        index++;
        iterator = s[index];  
    }

    if(RFormatInstructions.find(temp[0]) != RFormatInstructions.cend()){
        RInstructionExecutor(temp[0], temp[1], temp[2], temp[3]);
        lineNumber++;
    }
    else if(IFormatInstructions1.find(temp[0]) != IFormatInstructions1.cend()){
        if(regNameToRegNum.find(temp[2]) != regNameToRegNum.end()){
            IInstructionExecutor1(temp[0], temp[1], temp[2], temp[3], lineNumber, in);
            lineNumber++;
        }
        else{
            IInstructionExecutor1(temp[0], temp[1], temp[3], temp[2], lineNumber, in);
        }
    }
    else if(IFormatInstructions2.find(temp[0]) != IFormatInstructions2.cend()){
        IInstructionExecutor2(temp[0], temp[1], temp[3], temp[2]);
        lineNumber++;
    }
    else if(SFormatInstructions.find(temp[0]) != SFormatInstructions.cend()){
        SInstructionExecutor(temp[0], temp[3], temp[1], temp[2]);
        lineNumber++;
    }
    else if(BFormatInstructions.find(temp[0]) != BFormatInstructions.cend()){
        BInstructionExecutor(temp[0], temp[1], temp[2], temp[3], lineNumber, in);
    }
    else if(UFormatInstructions.find(temp[0]) != UFormatInstructions.cend()){
        string decimalOffset = "";
        if(temp[2].substr(0, 2) == "0x"){
            // temp[2] = temp[2].substr(temp[2].size() - 5); 
            // Last 5 hexadecimal bits are considered == 20 binary bits.
            temp[2] = temp[2].substr(2);
            reverse(temp[2].begin(), temp[2].end());
            int size = temp[2].size();
            for (int i = 0; i < (5 - size); i++){
                temp[2] += "0";
            }
            reverse(temp[2].begin(), temp[2].end());
            temp[2] = temp[2].substr(temp[2].size() - 5);
            decimalOffset = hexadecimalToDecimal(temp[2]);
        }
        else{
            decimalOffset = temp[2];
        }
        UInstructionExecutor(temp[0], temp[1], decimalOffset);
        lineNumber++;
    }
    else if(JFormatInstructions.find(temp[0]) != JFormatInstructions.cend()){
        JInstructionExecutor(temp[0], temp[1], temp[2], lineNumber, in);
    }
    regNumToRegValue[0] = "0000000000000000";
    updateCallStack(lineNumber - 1, 0);
}

void RInstructionExecutor(string op, string rd, string rs1, string rs2){
    rs1 = regNumToRegValue[regNameToRegNum[rs1]];
    rs2 = regNumToRegValue[regNameToRegNum[rs2]];
    string result = RFunctionMap[op](rs1, rs2);
    regNumToRegValue[regNameToRegNum[rd]] = result;
    regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
}

void IInstructionExecutor1(string op, string rd, string rs1, string imm, int& lineNumber, ifstream& in){
    imm = immediateGenerator(imm);
    rs1 = regNumToRegValue[regNameToRegNum[rs1]];
    string result = IFunctionMap[op](rs1, imm);
    
    if(op == "jalr"){
        regNumToRegValue[regNameToRegNum[rd]] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
        lineNumber = (stoi(hexadecimalToDecimal(result)) / 4) + startOfTextSeg;
        jumpToLine(in, lineNumber);
        regNumToRegValue[32] = result;
        updateCallStack(lineNumber, 2);
    }
    else{
        regNumToRegValue[regNameToRegNum[rd]] = result;
        regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
    }
}

void IInstructionExecutor2(string op, string rd, string rs1, string offset){
    offset = immediateGenerator(offset);
    rs1 = regNumToRegValue[regNameToRegNum[rs1]];

    string effectiveAddress = RFunctionMap["add"](rs1, offset);
    string result = "";
    int numberOfBytes = 0;

    if(op == "ld") numberOfBytes = 8;
    else if(op == "lw") numberOfBytes = 4;
    else if(op == "lh") numberOfBytes = 2;
    else if(op == "lb") numberOfBytes = 1;
    else if(op == "lbu") numberOfBytes = 1;
    else if(op == "lhu") numberOfBytes = 2;
    else if(op == "lwu") numberOfBytes = 4;

    for (size_t i = 0; i < numberOfBytes; i++){
        if(memory[effectiveAddress] == ""){
            result = "00" + result;
        }
        else{
            result = memory[effectiveAddress] + result;
        }
        effectiveAddress = RFunctionMap["add"](effectiveAddress, "0000000000000001");
    }
    
    char sign = result[0];
    if(op == "lbu" || op == "lhu" || op == "lwu") sign = '0';
    else{
        if(sign == '8' || sign == '9' || sign == 'a' || sign == 'b' || sign == 'c' || sign == 'd' || sign == 'e' || sign == 'f'){
            sign = 'f';
        }
        else{
            sign = '0';
        }
    }

    for (size_t i = 0; i < (16 - (2 * numberOfBytes)); i++){
        result = sign + result;
    }

    regNumToRegValue[regNameToRegNum[rd]] = result;
    regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
}

void SInstructionExecutor(string op, string rs1, string rs2, string offset){
    offset = immediateGenerator(offset);
    rs1 = regNumToRegValue[regNameToRegNum[rs1]];
    rs2 = regNumToRegValue[regNameToRegNum[rs2]];
    string effectiveAddress = RFunctionMap["add"](rs1, offset);
    // We are following little endian format, lsb at lower address.
    int numberOfBytes = 0;
    if(op == "sd") numberOfBytes = 8;
    else if(op == "sw") numberOfBytes = 4;
    else if(op == "sh") numberOfBytes = 2;
    else if(op == "sb") numberOfBytes = 1;

    for (int i = numberOfBytes - 1; i >= 0; i--){
        memory[effectiveAddress] = rs2.substr(2 * i, 2);
        effectiveAddress = RFunctionMap["add"](effectiveAddress, "0000000000000001");
    }

    regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
}

void BInstructionExecutor(string op, string rs1, string rs2, string label, int& lineNumber, ifstream& in){
    rs1 = regNumToRegValue[regNameToRegNum[rs1]];
    rs2 = regNumToRegValue[regNameToRegNum[rs2]];

    bool branchCondition = BFunctionMap[op](rs1, rs2);
    int offset = (labelData[label] - lineNumber) * 4;
    string Offset = to_string(offset);
    Offset = immediateGenerator(Offset);
    if(branchCondition == true){
        lineNumber = labelData[label];
        jumpToLine(in, lineNumber);
        regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], Offset);
    }
    else{
        regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
        lineNumber++;
    }
    
}

void JInstructionExecutor(string op, string rd, string label, int& lineNumber, ifstream& in){
    
    int offset = (labelData[label] - lineNumber) * 4;
    string Offset = to_string(offset);
    Offset = immediateGenerator(Offset);
    lineNumber = labelData[label];
    jumpToLine(in, lineNumber);
    regNumToRegValue[regNameToRegNum[rd]] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
    regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], Offset);
    updateCallStack(lineNumber, 1, label);
}

void UInstructionExecutor(string op, string rd, string imm){
    imm = immediateGenerator(imm);
    // cout << imm << endl;
    imm = RFunctionMap["sll"](imm, "000000000000000c");
    if(op == "lui"){
        regNumToRegValue[regNameToRegNum[rd]] = imm;
    }
    else{
        regNumToRegValue[regNameToRegNum[rd]] = RFunctionMap["add"](regNumToRegValue[32], imm);
    }
    regNumToRegValue[32] = RFunctionMap["add"](regNumToRegValue[32], "0000000000000004");
}

string hexadecimalToDecimal(string s){
    int multiplier = 1;
    int decimalValue = 0;
    for (int i = s.size() - 1; i >= 0; i--){
        if(s[i] >= 48 && s[i] <= 57){
            decimalValue += (s[i] - '0') * multiplier;
        }
        else{
            decimalValue += (s[i] - 'a' + 10) * multiplier;
        }
        multiplier *= 16;
    }
    
    return to_string(decimalValue);
}

string decimalToHexadecimal(string s){
    string result = decimalToBinary(s);
    result = binaryToHexadecimal(result);
    return result;
}

/**
 * @brief Removes spaces in the start and end of a string.
 * @param s A string
 * @return parsed string with no spaces in the start and end.
 */
string lineParser(string s){
    if(s == "") return "";

    int index = 0;
    int numberOfWhitespaces = 0;
    while (true){
        if(s[index] != ' ') break;
        else numberOfWhitespaces += 1;
        index++;
    }
    
    s = s.substr(numberOfWhitespaces);

    index = s.size() - 1;
    numberOfWhitespaces = 0;
    while (true){
        if(s[index] != ' ') break;
        else numberOfWhitespaces += 1;
        index--;
    }
    s = s.substr(0,(s.size() - numberOfWhitespaces));

    return s;
}

void setRegistersToZero(){
    string zero = "0000000000000000";
    for (size_t i = 0; i < 33; i++){
        regNumToRegValue[i] = zero;
    }
}

string step(ifstream& in, int& lineNumber){
    if(in.eof()){
        return "";
    }

    string s;
    string currentPC = regNumToRegValue[32];
    getline(in, s);
    s = lineParser(s);

    executeInstruction(s, lineNumber, in);

    int index = 0;
    int spaceFlag = 0;
    string temp = "";
    while (s[index] != '\0'){
        if(s[index] == ':'){
            temp = "";
            spaceFlag = 1;
        }
        else if(s[index] == ' ' && spaceFlag == 0){
            temp += s[index];
            spaceFlag = 1;
        }
        else if(s[index] != ' '){
            temp += s[index];
            spaceFlag = 0;
        }
        index++;
    }
    
    cout << "Executed: " << temp << "; PC=0x" << currentPC.substr(8) << endl;

    return s;
}

void run(ifstream& in, int& lineNumber){
    string temp = "";

    while (true){
        if(breakPoints.find(lineNumber) != breakPoints.cend()){
            cout << "Execution stopped at break point: " << lineNumber << endl;
            break; 
        }
        temp = step(in, lineNumber);
        if(temp == ""){
            cout << "Execution reached end of the file" << endl;
            break;
        }
    }
    
}

bool setBreakPoint(int atLine){
    breakPoints.insert(atLine);
    cout << "Break point set at line number: " << atLine << endl;
    return true;
}

bool removeBreakPoint(int atLine){
    if(breakPoints.find(atLine) != breakPoints.cend()){
        breakPoints.erase(atLine);
        cout << "Removed break point at line: " << atLine << endl;
    }
    else{
        cout << "Break point was not set at line: "<<atLine<<" in the first place." << endl;
    }
    return true;
}

void clearBreakPoint(){
    breakPoints.clear();
}

void printMemory(string address, int numberOfBytes){

    string byte = "";
    address = address.substr(2);
    string address64bits = "00000000000" + address;

    for (size_t i = 0; i < numberOfBytes; i++){
        byte = memory[address64bits];
        if(byte == "") byte = "00";
        cout << "Memory[0x" << address << "] = 0x" << byte << endl;

        address64bits = RFunctionMap["add"](address64bits, "0000000000000001");
        address = address64bits.substr(address64bits.size() - 5);
    }
    
}


void jumpToLine(ifstream& in, int& lineNumber){
    in.clear();
    in.seekg(0);

    string s = "";
    for (size_t i = 0; i < lineNumber - 1; i++){
        getline(in, s);
        s = "";
    }

}

void printRegisterValues(){
    cout << "Registers: " << endl;
    for (size_t i = 0; i < 32; i++){
        if(i < 10){
            cout << "x" << i << "  = " <<"0x"<< regNumToRegValue[i] << endl;
        }
        else{
            cout << "x" << i << " = " <<"0x"<< regNumToRegValue[i] << endl;
        }
    }
}