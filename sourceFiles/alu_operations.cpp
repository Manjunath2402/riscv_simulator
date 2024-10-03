#include "./alu_prototypes.hh"

map<char, int> hexToDec = {{'0', 0}, {'1', 1}, {'2', 2}, {'3', 3},
                           {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
                           {'8', 8}, {'9', 9}, {'a', 10}, {'b', 11},
                           {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}};

map<int, char> decToHex = {{0, '0'}, {1, '1'}, {2, '2'}, {3, '3'},
                           {4, '4'}, {5, '5'}, {6, '6'}, {7, '7'},
                           {8, '8'}, {9, '9'}, {10, 'a'}, {11, 'b'},
                           {12, 'c'}, {13, 'd'}, {14, 'e'}, {15, 'f'}};

map<char, string> hexToBin = {
    {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
    {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
    {'8', "1000"}, {'9', "1001"}, {'a', "1010"}, {'b', "1011"},
    {'c', "1100"}, {'d', "1101"}, {'e', "1110"}, {'f', "1111"}
};

map<string, string> binToHex = {
    {"0000", "0"}, {"0001", "1"}, {"0010", "2"}, {"0011", "3"},
    {"0100", "4"}, {"0101", "5"}, {"0110", "6"}, {"0111", "7"},
    {"1000", "8"}, {"1001", "9"}, {"1010", "a"}, {"1011", "b"},
    {"1100", "c"}, {"1101", "d"}, {"1110", "e"}, {"1111", "f"}
};

/**
 * @brief This is function is used by both add and addi function.
 * @param op1, op2 Hexadecimal strings of 16 bits length.
 * @return returns a hexadecimal string of 16 bits length which contains result of op1 + op2.
 */
string _add(string op1, string op2){
    int index = 0;
    string result = "";

    reverse(op1.begin(), op1.end());
    reverse(op2.begin(), op2.end());

    int n1 = 0, n2 = 0;
    int carry = 0;
    while (index < 16){
        n1 = hexToDec[op1[index]];
        n2 = hexToDec[op2[index]];
        result = result + decToHex[(n1 + n2 + carry) % 16];
        carry = (n1 + n2 + carry)/16;
        index++;
    }
    
    reverse(result.begin(), result.end());

    return result;
}

/**
 * @brief This function is used by sub instruction.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns a hexadecimal string of length 16 bits which contains result of op1 - op2.
 */
string _sub(string op1, string op2){
    int index = 0;
    string result = "";

    reverse(op1.begin(), op1.end());
    reverse(op2.begin(), op2.end());

    int n1 = 0, n2 = 0;
    int borrow = 0;
    while (index < 16){
        n1 = hexToDec[op1[index]];
        n2 = hexToDec[op2[index]];
        if(n1 >= (n2 + borrow)){
            result += decToHex[(n1 - n2 - borrow) % 16];
            borrow = 0;
        }
        else{
            result += decToHex[(n1 + 16 - n2 - borrow) % 16];
            borrow = 1;
        }
        index++;
    }
    
    reverse(result.begin(), result.end());

    return result;
}

/**
 * @brief This function is used by both xor and xori.
 * @param op1, op2 Hexadecimal string of length 16 bits.
 * @return returns a hexadecimal strig of length 16 bits which contains result of op1 xor op2.
 */
string _xor(string op1, string op2){
    int index = 0;
    string result = "";

    op1 = hexadecimalToBinary(op1);
    op2 = hexadecimalToBinary(op2);

    while (index < 64){
        if(op1[index] == op2[index]){
            result += '0';
        }
        else{
            result += '1';
        }
        index++;
    }
    
    result = binaryToHexadecimal(result);

    return result;
}

/**
 * @brief This function is used both or and ori.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns a Hexadecimal string of length 16 bits which contains result of op1 or op2.
 */
string _or(string op1, string op2){
    int index = 0;
    string result = "";

    op1 = hexadecimalToBinary(op1);
    op2 = hexadecimalToBinary(op2);

    while (index < 64){
        if(op1[index] == '0' && op2[index] == '0'){
            result += '0';
        }
        else{
            result += '1';
        }
        index++;
    }
    
    result = binaryToHexadecimal(result);

    return result;
}

/**
 * @brief This function is used by both and and andi.
 * @param op1, op2 Hexadecimal string of length 16 bits.
 * @return returns a hexadecimal string of length 16 bits which contains result of op1 and op2.
 */
string _and(string op1, string op2){
    int index = 0;
    string result = "";

    op1 = hexadecimalToBinary(op1);
    op2 = hexadecimalToBinary(op2);

    while (index < 64){
        if(op1[index] == '1' && op2[index] == '1'){
            result += '1';
        }
        else{
            result += '0';
        }
        index++;
    }

    result = binaryToHexadecimal(result);

    return result;
}

/**
 * @brief This function used by both sll and slli.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns a Hexadecimal string of length 16 bits which contains the value op1 << op2
 */
string _sll(string op1, string op2){
    /*
    A value can be shifted at max 64 bits for slli, for sll we will consider only first
    two bits of op2 since first two bits can accomodate the 64 and op1 is shifted accordingly. Even if
    op2 contains a some large value in it we will only consider first 2 bits. Just like in Ripes.
    If the second bit contains 4 or any higher value we will consider only 1 bit, lsb of op2.
    */

    string shift = op2.substr(14, 2);
    string result = "";
    int shiftBy = 0;
    // only last two bits.
    if(shift[0] == '1' || shift[0] == '2' || shift[0] == '3'){
        shiftBy += int(shift[0] - '0') * 16;
    }
    if(shift[1] >= 48 && shift[1] <= 57){
        shiftBy += int(shift[1] - '0');
    }
    else{
        shiftBy += int(shift[1] - 'a' + 10);
    }
    

    op1 = hexadecimalToBinary(op1);
    for (size_t i = 0; i < shiftBy; i++){
        op1 += '0';
    }
    
    result = op1.substr(shiftBy, 64);
    result = binaryToHexadecimal(result);

    return result;
}

/**
 * @brief This function is used by both srl and srli.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns a Hexadecimal string of length 16 bits which contains value of op1 >> op2
 */
string _srl(string op1, string op2){
    /*
    Just as sll except we shift right this time.
    */
    string result = "";
    int shiftBy = 0;
    string shift = op2.substr(14, 2);

    if(shift[0] == '1' || shift[0] == '2' || shift[0] == '3'){
        shiftBy = int(shift[0] - '0') * 16;
    }
    if(shift[1] >= 48 && shift[1] <= 57){
        shiftBy += int(shift[1] - '0');
    }
    else{
        shiftBy += int(shift[1] - 'a' + 10);
    }

    op1 = hexadecimalToBinary(op1);
    reverse(op1.begin(), op1.end());

    for (size_t i = 0; i < shiftBy; i++){
        op1 += '0';
    }
    
    op1 = op1.substr(shiftBy, 64);
    reverse(op1.begin(), op1.end());

    result = binaryToHexadecimal(op1);

    return result;
}

/**
 * @brief This function is used by sra and srai.
 * @param op1, op2 Hexadecimal string of length 16 bits.
 * @return returns a Hexadecimal string of length 16 bits which contains op1 >> op2(sign extension). 
 */
string _sra(string op1, string op2){
    string result = "";
    int shiftBy = 0;
    string shift = op2.substr(14, 2);

    if(shift[0] == '1' || shift[0] == '2' || shift[0] == '3'){
        shiftBy = int(shift[0] - '0') * 16;
    }
    if(shift[1] >= 48 && shift[1] <= 57){
        shiftBy += int(shift[1] - '0');
    }
    else{
        shiftBy += int(shift[1] - 'a' + 10);
    }

    char sign = hexToBin[op1[0]][0];

    op1 = hexadecimalToBinary(op1);
    reverse(op1.begin(), op1.end());

    for (size_t i = 0; i < shiftBy; i++){
        op1 += sign;
    }
    
    op1 = op1.substr(shiftBy, 64);
    reverse(op1.begin(), op1.end());

    result = binaryToHexadecimal(op1);

    return result;
}

/**
 * @brief The ALU set less than operation. which sets given destination to 1 when the op1 is less than op2. else sets destionation to 0.
 * @param op1, op2 Hexadecimal strings of 16 bits length that are to be compared. comparison(op1 < op2).
 * @return returns a hexadecimal string of length which contain 1 is comparison is true else it is put to 0.
 */
string _slt(string op1, string op2){
    bool result = _isLessThan(op1, op2);
    if(result == true){
        return "0000000000000001";
    }
    return "0000000000000000";
}

/**
 * @brief The ALU set less than unsigned operation. which sets given destination to 1 when the op1 is less than op2,
 * else sets destionation to 0. Unsigned comparison is followed.
 * @param op1, op2 Hexadecimal strings of 16 bits length that are to be compared. comparison(op1 < op2).
 * @return returns a hexadecimal string of length which contain 1 is comparison is true else it is put to 0.
 */
string _sltu(string op1, string op2){
    bool result = _isLessThanUn(op1, op2);
    if(result == true){
        return "0000000000000001";
    }
    return "0000000000000000";
}

// comparators.
/**
 * @brief This function is used by beq.
 * @param op1, op2 Hexadecmal strings of length 16 bits.
 * @return returs true if both operands are equal, false otherwise.
 */
bool _isEqual(string op1, string op2){
    /*
    If two operand are equal then result of op1 xor op2 will be 0.
    we will utilize xor for this function.
    */
    bool result = false;

    string xor_result = _xor(op1, op2);
    if(xor_result == "0000000000000000") result = true;

    return result;
}

/**
 * @brief This function is used by bne.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns true if both are not equal and false if equal.
 */
bool _isNotEqual(string op1, string op2){
    // we will _isEqual function in this.

    return !(_isEqual(op1, op2));
}

/**
 * @brief This function is used by blt.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns true if op1 is strictly less than op2, 0 otherwise
 */
bool _isLessThan(string op1, string op2){
    /*
    This function should only be used for signed comparison.
    */
    bool result = false;
    char msb_op1 = hexToBin[op1[0]][0];
    char msb_op2 = hexToBin[op2[0]][0];
    
    if(msb_op1 == '1' && msb_op2 == '0') result = true;
    else if(msb_op1 == '0' && msb_op2 == '1') result = false;
    else{
        for (size_t i = 0; i < 16; i++){
            if(hexToDec[op1[i]] < hexToDec[op2[i]]) {
                result = true;
                break;
            } 
            else if(hexToDec[op1[i]] > hexToDec[op2[i]]){
                break;
            }
        }
    }

    return result;
}

/**
 * @brief This function is used by bge.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return return true if op1 is greater than or equal to op2, false otherwise.
 */
bool _isGreaterOrEqual(string op1, string op2){
    return !(_isLessThan(op1, op2));
}

/**
 * @brief same as _isLessThan except we compare registers in unsigned way.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns true if op1 is strictly less than op2, false otherwise
 */
bool _isLessThanUn(string op1, string op2){
    bool result = false;
    for (size_t i = 0; i < 16; i++){
        if(hexToDec[op1[i]] > hexToDec[op2[i]]){
            result = false;
            break;
        }
        else if(hexToDec[op1[i]] < hexToDec[op2[i]]){
            result = true;
            break;
        }
    }
    return result;
}

/**
 * @brief same as _isGreaterOrEqual except we compare resiters in unsigned way.
 * @param op1, op2 Hexadecimal strings of length 16 bits.
 * @return returns true if op1 is greater than or equal to op2, false other wise.
 */
bool _isGreaterOrEqualUn(string op1, string op2){
    return !(_isLessThanUn(op1, op2));
}

/**
 * @brief Given a hexadecimal string return a binary equivalent of it.
 * @param op1 a hexadecimal string of length 16 bits.
 * @return a binary equivalent in the form a string.
 */
string hexadecimalToBinary(string op1){
    int index = 0;
    string result = "";

    while (index < 16){
        result += hexToBin[op1[index]];
        index++;
    }

    return result;
}

/**
 * @brief Given a binary string return a hexadecimal equivalent of it.
 * @param op1 hexadecimal string of length 16 bits.
 * @return a hexadecimal equivalent in form of a string.
 */
string binaryToHexadecimal(string op1){
    int index = 0;
    string result = "";
    string temp = "";

    while (index <= 60){
        temp += op1[index];
        temp += op1[index + 1];
        temp += op1[index + 2];
        temp += op1[index + 3];

        result += binToHex[temp];
        index += 4;
        temp = "";
    }
    
    return result;
}