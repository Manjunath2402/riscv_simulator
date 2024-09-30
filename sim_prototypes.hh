#include "./alu_prototypes.hh"

void printRegisterValues();

string immediateGenerator(string );
string decimalToBinary(string );
string hexadecimalToDecimal(string );

void labelParser(ifstream& );
void executeInstruction(string, int);

void RInstructionExecutor(string, string, string, string);
void IInstructionExecutor1(string, string, string, string);
void IInstructionExecutor2(string, string, string, string);
void SInstructionExecutor(string, string, string, string);
void BInstructionExecutor(string, string, string, string, int);
void UInstructionExecutor(string, string, string);