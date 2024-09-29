/**
 * @brief This file is going to contain prototypes of all operations. 
 */

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

/**
 * These all are functions of ALU, which takes arguments of hexadecimal strings of length 16 bits.
 */
string _add(string, string);
string _sub(string, string);
string _xor(string, string);
string _or(string, string);
string _and(string, string);
string _sll(string, string);
string _srl(string, string);
string _sra(string, string);
string _slt(string, string);
string _sltu(string, string);

/**
 * comparators for ALU 
 */
bool _isEqual(string, string);
bool _isNotEqual(string, string);
bool _isLessThan(string, string);
bool _isGreaterOrEqual(string, string);
bool _isLessThanUn(string, string);
bool _isGreaterOrEqualUn(string, string);

string binaryToHexadecimal(string);
string hexadecimalToBinary(string);