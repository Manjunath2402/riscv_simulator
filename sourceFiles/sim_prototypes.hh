#include "./cache_prototypes.hh"

class functionStack{
    string label;
    int lineNumber;

    public:
    functionStack(string s, int l) : label(s), lineNumber(l) {}
    functionStack() : label(""), lineNumber(0) {}

    void updateLine(int l){
        this->lineNumber = l;
    }

    void updateLabel(string s){
        this->label = s;
    }

    string showLabel(){
        return this->label;
    }

    int showLineNumber(){
        return this->lineNumber;
    }

    functionStack& operator=(functionStack f){
        this->label = f.label;
        this->lineNumber = f.lineNumber;
        return *this;
    }
};

string immediateGenerator(string );
string decimalToBinary(string );
string hexadecimalToDecimal(string );
string decimalToHexadecimal(string );

void labelParser(ifstream& );
string lineParser(string );
void setRegistersToZero();
void setMemoryToZero();
void initialiseDataSegment(ifstream&);
void setBufferFromTextSeg(ifstream& , int&);

string step(ifstream& , int&);
void run(ifstream&, int&);
void createCallStack();
void updateCallStack(int, int, string);
bool setBreakPoint(int );
bool removeBreakPoint(int );
void clearBreakPoint();

void printMemory(string, int);
void printCallStack();
void printRegisterValues();

void executeInstruction(string, int&, ifstream&);
void RInstructionExecutor(string, string, string, string);
void IInstructionExecutor1(string, string, string, string, int&, ifstream& );
void IInstructionExecutor2(string, string, string, string);
void SInstructionExecutor(string, string, string, string);
void BInstructionExecutor(string, string, string, string, int&, ifstream& );
void JInstructionExecutor(string, string, string, int&, ifstream&);
void UInstructionExecutor(string, string, string);

void jumpToLine(ifstream&, int&);