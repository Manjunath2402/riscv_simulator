#include "./sim_prototypes.hh"

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

    {"fp", 8}
};

map<int, string> regNumToRegValue = {
    {0, "0000000000000000"}, {1, "0000000000000000"}, {2, "0000000000000000"}, {3, "0000000000000000"},
    {4, "0000000000000000"}, {5, "0000000000000000"}, {6, "0000000000000000"}, {7, "0000000000000000"}, 
    {8, "0000000000000000"}, {9, "0000000000000000"}, {10, "0000000000000000"}, {11, "0000000000000000"}, 
    {12, "0000000000000000"}, {13, "0000000000000000"}, {14, "0000000000000000"}, {15, "0000000000000000"}, 
    {16, "0000000000000000"}, {17, "0000000000000000"}, {18, "0000000000000000"}, {19, "0000000000000000"}, 
    {20, "0000000000000000"}, {21, "0000000000000000"}, {22, "0000000000000000"}, {23, "0000000000000000"}, 
    {24, "0000000000000000"}, {25, "0000000000000000"}, {26, "0000000000000000"}, {27, "0000000000000000"}, 
    {28, "0000000000000000"}, {29, "0000000000000000"}, {30, "0000000000000000"}, {31, "0000000000000000"}
};


/**
 * @brief This function is used by immediate Instructions.
 * @param op1 a 
 */
string immediateGenerator(string op1){
    string result = decimalToBinary(op1);
    reverse(result.begin(), result.end());
    result = result.substr(0, 12);

    char sign = result[11];
    for (size_t i = 0; i < 52; i++){
        result += sign;
    }
    
    reverse(result.begin(), result.end());
    result = binaryToHexadecimal(result);

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
    while (bin.size() < 21)
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