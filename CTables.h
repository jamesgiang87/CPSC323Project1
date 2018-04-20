// Date: 4/17/2018
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
// File: CTables.h
//*** NEED TO CHECK ALL VARIABLE AND SEE HOW TO CONNECT THEM TO INT/BOOLEAN/REAL because they
//    would most likely appear as identifiers i believe
//*** NEED TO MAKE SURE THAT ERRORS ARE CAUGHT FOR THESE FUNCTIONS IN CParser.cpp

#ifndef CTables_HEADER
#define CTables_HEADER

#include "CLexer.h"

const int SYMBOL_TABLE_SIZE        = 1000;
const int INSTRUCTION_TABLE_SIZE   = 1000;
const bool PRINT_SYMBOL_TABLE 	   = true;
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
        NONE,
        REDECLARATION,      // variable is being redeclared
        UNDECLARED,         // variable used when not declared
        MISMATCH,           // mismatched variable type (bool = real)
        MEM_OUT_OF_RANGE,   // memory is from => 2000+
        MAX_RANGE_REACHED   // only stores SYMBOL_TABLE_SIZE
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
    
        void SetRecVarTypeUsed(const VariableTypes varType)
                                                {m_recVarTypeUsed = varType;}
        void SetDeclaringVar(const bool state) {m_declaringVar = state;}
        void PrintError(const Error_SymbolTable error,const CLexer& token)const;
    
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
        int GetMemAddr(const std::string varName);
        void SetVarTypeUsed(const VariableTypes varTypeUsed);
        void SetVarName(const std::string varName, const int indexNum);
        void SetMemAddr(const int memAddr, const int indexNum);
        void SetVarType(const VariableTypes varType, const int indexNum);
    
        void SetError(const Error_SymbolTable error) {m_error = error;}
        Error_SymbolTable GetError() {return m_error;}
    
        bool IsDeclaringVar() const {return m_declaringVar;}
    
    // PRIVATE DATA MEMBERS
        STI m_table[SYMBOL_TABLE_SIZE]; // stores all table data
        int m_curIndex;                 // the current index num
        int m_curMemAddr;               // the current memory address
        VariableTypes m_recVarTypeUsed; // most recent variable declared
        bool m_declaringVar;            // is variable being declared
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
    int operand;
};


class CInstructionTable
{
    public:
        CInstructionTable();
        void GenerateInstr(const InstructionType instType, const int addr = 0);
        std::string GetInstrStr(const int instrNum);
        int GetAddress(const int index) const {return m_table[index].addr;}
        int GetCurIndex() const {return m_curIndex;}
        std::string GetInstr(const int index)const{return m_table[index].instr;}
        int GetOperand(const int index) const {return m_table[index].operand;}
        void SetAddress(const int addr, const int index);
        void SetCurIndex(const int index) {m_curIndex = index;}
        void SetInstr(const std::string instr, const int index);
        void SetOperand(const int operand, const int index);
        void PrintTable() const;
    
    private:
        ITI m_table[INSTRUCTION_TABLE_SIZE];
        int m_curIndex;
};


#endif /* CTables_hpp */

