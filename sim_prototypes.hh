#include "./alu_prototypes.hh"

void printRegisterValues();

string immediateGenerator(string );
string decimalToBinary(string );
string hexadecimalToDecimal(string );

void labelParser(ifstream& );
string lineParser(string );
void setRegistersToZero();
string step(ifstream& , int&);
void run(ifstream&, int&);
void getPC();
void executeInstruction(string, int&, ifstream&);

void RInstructionExecutor(string, string, string, string);
void IInstructionExecutor1(string, string, string, string);
void IInstructionExecutor2(string, string, string, string);
void SInstructionExecutor(string, string, string, string);
void BInstructionExecutor(string, string, string, string, int&, ifstream& );
void JInstructionExecutor(string, string, string, int&, ifstream&);
void UInstructionExecutor(string, string, string);

void jumpToLine(ifstream&, int&);