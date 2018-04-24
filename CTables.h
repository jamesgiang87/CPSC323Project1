// Date: 4/17/2018
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
// File: CTables.h

//*** WHAT DO YOU MEAN BY MODIFY PARSER TO NOT INCLUDE FUNCTIONS AND REALS? IS ISSUEING
//    ERROR FOR THOSE OK? OR NEED TO REMOVE THEM?
//*** THE VIRTUAL MACHINE WILL USE THE STACK WE DO NOT NEED TO IMPLEMENT THAT CORRECT?
//*** ASK TEACHER NOT SURE FOR - <PRIMARY> EXPRESSION CAN WE DO PUSHI -1 and then MUL?
//    IS THAT ALLOWED? YOU DONT GIVE ANY EXAMPLES
//*** ASK TEACHER NEED TO HAVE MULTIPLE STDIN TO GET MULTIPLE VALUES. SO ONE STDIN GETS ONLY ONE VALUE FROM
//    INPUT
//*** -true or -false should be an error because boolean arithmetic?
//*** NEED TO MAKE SURE THAT ERRORS ARE CAUGHT FOR THESE FUNCTIONS IN CParser.cpp

#ifndef CTables_HEADER
#define CTables_HEADER

#include "CLexer.h"
#include <stack>

const int  SYMBOL_TABLE_SIZE       = 30;
const int  INSTRUCTION_TABLE_SIZE  = 30;
const bool PRINT_SYMBOL_TABLE      = true;
const bool PRINT_INSTRUCTION_TABLE = true;

// keeps track of what variable types we have found (when declared)
enum VariableTypes
{
  NO_TYPE=0,
  INT,
  REAL,
  BOOLEAN
};

const std::string VariableType[4] = {"NO_TYPE", "integer", "real", "boolean"};

enum Error_SymbolTable
{
        ST_NONE,
        ST_BOOLEAN_ARITHMETIC,  // there is no boolean arithmetic allowed
        ST_BOOLEAN_EXPECTED,    // a true or false token expected
        ST_REAL_USED,           // reals cannot be used for this assignment
        ST_REDECLARATION,       // variable is being redeclared
        ST_UNDECLARED,          // variable used when not declared
        ST_OPERATION_MISMATCH,  // mismatched variable type (bool = real)
        ST_MEM_OUT_OF_RANGE,    // memory is from => 2000+
        ST_MAX_RANGE_REACHED    // only stores SYMBOL_TABLE_SIZE
    
};


//Symbol Table Index
struct STI
{
    std::string m_varName;   // stores the lexeme of the variable
    int m_memAddr;           // stores the memory address
    VariableTypes m_varType; // the type of variable (ex. int, bool)
};


class CSymbolTable
{
    public:
        CSymbolTable();
    
        void Insert(const CToken& variable);
        void List() const;
    
        void SetExpVarTypeUsed(const VariableTypes varType);
        void SetRecVarTypeUsed(const VariableTypes varType);
        void SetDeclaringVar(const bool state) {m_declaringVar = state;}
        void PrintError(const Error_SymbolTable error,const CLexer& token)const;
        VariableTypes GetVarType(const std::string varName);
        VariableTypes GetExpVarTypeUsed() {return m_ExpVarTypeUsed;}
        std::string GetExpVarTypeUsedStr() const;
        bool IsDeclaringVar() const {return m_declaringVar;}

        int GetMemAddr(const std::string varName);
    
    
    private:
    //PRIVATE MEMBER FUNCTIONS
        bool LookUp(const CToken& variable) const;
        bool LookUp(const std::string varName, int& index) const;
    
        int GetCurIndex()   const         {return m_curIndex;}
        int GetCurMemAddr() const         {return m_curMemAddr;}
        void SetCurIndex(const int num)   {m_curIndex = num;}
        void SetCurMemAddr(const int num) {m_curMemAddr = num;}
   
        // gets the variable type most recenetly used
        VariableTypes GetRecVarTypeUsed() const {return m_recVarTypeUsed;}
        std::string GetRecVarTypeUsedStr() const
                                    {return VariableType[GetRecVarTypeUsed()];}
        std::string GetVarName(const int indexNum) const;
        VariableTypes GetVarType(const int indexNum) const;
        std::string GetVarTypeStr(const int indexNum) const;
        int GetMemAddr(const int indexNum) const;
        void SetVarName(const std::string varName, const int indexNum);
        void SetMemAddr(const int memAddr, const int indexNum);
        void SetVarType(const VariableTypes varType, const int indexNum);
    
        void SetError(const Error_SymbolTable error) {m_error = error;}
        Error_SymbolTable GetError() {return m_error;}
    
    // PRIVATE DATA MEMBERS
        STI m_table[SYMBOL_TABLE_SIZE]; // stores all table data
        int m_curIndex;                 // the current index num
        int m_curMemAddr;               // the current memory address
        VariableTypes m_recVarTypeUsed; // most recent variable declared
        bool m_declaringVar;            // is variable being declared
        VariableTypes m_ExpVarTypeUsed; // type of variable used in expression
        Error_SymbolTable m_error;      // stores error information
    
};


enum InstructionType
{
    PUSHI=0, PUSHM, POPM, STDOUT, STDIN, ADD, SUB, MUL, DIV, GRT, LES, EQU, NEQ,
    GEQ, LEQ, JUMPZ, JUMP, LABEL
};


const std::string Instructions [18] =
{ "PUSHI", "PUSHM", "POPM", "STDOUT", "STDIN", "ADD", "SUB", "MUL", "DIV",
  "GRT", "LES", "EQU", "NEQ", "GEQ", "LEQ", "JUMPZ", "JUMP", "LABEL"
};


//Instruction Table Index
struct ITI
{
    int addr;
    std::string instr;
    std::string operand;
};

enum Error_InstrTable
{
    IT_NONE,
    IT_MAX_RANGE_REACHED       // only stores SYMBOL_TABLE_SIZE
};


class CInstructionTable
{
    public:
        CInstructionTable();
        void BackPatch(const int addr);
        void GenerateInstr(const InstructionType instType,
                           const std::string addr = "");
        std::string GetInstrStr(const int instrNum);
        int GetAddress(const int index);
        int GetCurIndex() const {return m_curIndex;}
        std::string GetInstr(const int index);
        std::string GetOperand(const int index);
        void SetAddress(const int addr, const int index);
        void SetCurIndex(const int index) {m_curIndex = index;}
        void SetInstr(const std::string instr, const int index);
        void SetOperand(const std::string operand, const int index);
        void PrintError(const Error_InstrTable error,const CLexer& token)const;
        void PrintTable();
        int PopJumpStack();
        void PushJumpStack(const int addr);
    
    private:
        ITI m_table[INSTRUCTION_TABLE_SIZE];
        int m_curIndex;
        std::stack<int> m_jumpStack;
        Error_InstrTable m_error;
    
        void SetError(const Error_InstrTable error) {m_error = error;}
        Error_InstrTable GetError() {return m_error;}
    
};


#endif /* CTables_HEADER */

