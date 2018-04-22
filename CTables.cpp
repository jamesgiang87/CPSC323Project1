// Date: 4/17/2018
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
// File: CTables.cpp

#include "CTables.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>


//============================ CSymbolTable ====================================
CSymbolTable::CSymbolTable(): m_curMemAddr(2000), m_recVarTypeUsed(NO_TYPE),
                              m_error(ST_NONE), m_curIndex(0),
                              m_declaringVar(false)
{
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        SetVarName("", GetCurIndex());
        SetMemAddr(0, GetCurIndex());
        SetVarType(NO_TYPE, GetCurIndex());
    }
}


void CSymbolTable::Insert(const CToken& variable)
{
    // check to see if the variable has been declared already
    if (!LookUp(variable))
    {
        SetVarName(variable.GetLexeme(), GetCurIndex());
        SetMemAddr(GetCurMemAddr(), GetCurIndex());
        SetVarType(GetRecVarTypeUsed(), GetCurIndex());
        if (GetVarTypeStr(GetCurIndex()) == VariableType[NO_TYPE])
        {
            SetError(ST_UNDECLARED);
            throw GetError();
        }
        
        SetCurIndex(GetCurIndex()+1);
        SetCurMemAddr(GetCurMemAddr()+1);
        
    }
    else
    {
        // if we found a variable already in the table and we are declaring
        //  a variable instance then throw error
        if (IsDeclaringVar())
        {
            SetError(ST_REDECLARATION);
            throw GetError();
        }
    }
}


void CSymbolTable::List() const
{
    //*** MAYBE FORMAT THIS USING SETW and LONGEST IDENTIFIER TO SPACE NEATLY
    if (PRINT_SYMBOL_TABLE)
    {
        //get the length of the largest variable
        long long varSize = 0;      //default space by 5
        
        for (int i = 0; i < GetCurIndex(); i++)
        {
            std::string tempStr = GetVarName(i);
            long long tempSize = tempStr.length();
            if (tempSize > i)
            {
                varSize = tempSize;
            }
        }
        if (varSize < strlen("identifier"))
        {
            varSize += strlen("identifier") - varSize;
        }
        
        varSize += 5;
        std::cout << std::left << std::setw(int(varSize))<<  "Identifier"
        << std::setw(strlen("Memory Location")+5)<< "Memory Location" << "Type" << std::endl;

        for (int i = 0; i < GetCurIndex(); i++)
        {
            std::cout << std::left << std::setw(int(varSize)) << GetVarName(i)
            << std::setw(strlen("Memory Location")+5) << GetMemAddr(i)
            << GetVarTypeStr(i) << "\n";
        }
    }
}


void CSymbolTable::SetRecVarTypeUsed(const VariableTypes varType)
{
    if (varType == REAL)
    {
        SetError(ST_REAL_USED);
        throw GetError();
    }
    else
    {
        m_recVarTypeUsed = varType;
    }
}


bool CSymbolTable::LookUp(const CToken& variable) const
{
    std::string varName = variable.GetLexeme();
    
    for (int i = 0; i < GetCurIndex(); i++)
    {
        if (varName == GetVarName(i))
        {
            return true;
        }
    }
    return false;
}


bool CSymbolTable::LookUp(const std::string varName, int& index) const
{
    for (index = 0; index < GetCurIndex(); index++)
    {
        if (varName == GetVarName(index))
        {
            return true;
        }
    }
    return false;
}


std::string CSymbolTable::GetVarName(const int indexNum) const
{
    return m_table[indexNum].m_varName;
    //return m_table[GetCurIndex()].m_varName;
}


VariableTypes CSymbolTable::GetVarType(const int indexNum) const
{
    return m_table[indexNum].m_varType;
}


std::string CSymbolTable::GetVarTypeStr(const int indexNum) const
{
    return VariableType[m_table[indexNum].m_varType];
}


int CSymbolTable::GetMemAddr(const int indexNum) const
{
    return m_table[indexNum].m_memAddr;
    //return m_table[GetCurIndex()].m_memAddr;
}


int CSymbolTable::GetMemAddr(const std::string varName) 
{
    int index = 0;
    if (LookUp(varName, index))
    {
        return GetMemAddr(index);
    }
    else
    {
        SetError(ST_UNDECLARED);
        throw GetError();
    }
    
    //*** NOTE: SHOULD NEVER OCCUR
    return -1;
}

void CSymbolTable::SetVarName(const std::string varName, const int indexNum)
{
    //*** NOTE: SHOULD NEVER set m_varName to a variable name thats already
    //          in the table
    m_table[indexNum].m_varName = varName;
}


void CSymbolTable::SetMemAddr(const int memAddr, const int indexNum)
{
        m_table[indexNum].m_memAddr = memAddr;
}


void CSymbolTable::SetVarType(const VariableTypes varType, const int indexNum)
{
    m_table[indexNum].m_varType = varType;
}


void CSymbolTable::PrintError(const Error_SymbolTable error,
                              const CLexer& token) const
{
    long long colmNum = 0;
    if (token.GetColmNum() > 0)
    {
        colmNum = token.GetColmNum() - token.CalcErrorOffset();
    }

    std::cout << std::endl;
    int index = 0;
    
    switch(error)
    {
        case ST_REAL_USED:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": declarations of type reals are not permitted"
            << std::endl;
            break;
            
        case ST_REDECLARATION:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": redeclaration of " << token.GetLexeme()
            << std::endl;
            break;
            
        case ST_UNDECLARED:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": undeclared identifier " << token.GetLexeme()
            << std::endl;
            break;
            
        case ST_OPERATION_MISMATCH:
            LookUp(token.GetLexeme(), index);
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": operation type mismatch of " << token.GetLexeme()
            << " is of type " << GetVarType(index) << std::endl;
            break;
            
        case ST_MEM_OUT_OF_RANGE:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": invalid symbol table memory address" << std::endl;
            break;
            
        case ST_MAX_RANGE_REACHED:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": max symbol table memory reached" << std::endl;
            break;
            
        default:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": unknown symbol table error occured" << std::endl;
            break;
    }
}


//============================ CInstructionTable ===============================
CInstructionTable::CInstructionTable() : m_curIndex(1), m_error(IT_NONE)
{
    for (int i = 0; i < INSTRUCTION_TABLE_SIZE; i++)
    {
        SetAddress(0, i);
        SetInstr("NONE", i);
        SetOperand("", i);
    }
}


void CInstructionTable::BackPatch(const int jumpAddr)
{
    int addr = PopJumpStack();
    SetOperand(std::to_string(jumpAddr), addr);
}

void CInstructionTable::GenerateInstr(const InstructionType instType,
                                      const std::string addr)
{
    SetInstr(GetInstrStr(instType), GetCurIndex()-1);
    SetAddress(GetCurIndex(), GetCurIndex()-1);
    SetOperand(addr, GetCurIndex()-1);
    SetCurIndex(GetCurIndex()+1);
}


std::string CInstructionTable::GetInstrStr(const int instrNum)
{
   return Instructions[instrNum];
}


void CInstructionTable::SetAddress(const int addr, const int index)
{
    m_table[index].addr = addr;
}


void CInstructionTable::SetInstr(const std::string instr, const int index)
{
    m_table[index].instr = instr;
}


void CInstructionTable::SetOperand(const std::string operand, const int index)
{
    if((GetInstr(index) == Instructions[POPM] ||
        GetInstr(index) == Instructions[PUSHM]) &&
       std::stoi(operand) < 2000)
    {
        //*** NEED TO THROW ERROR
        std::cout << "ERRORORORORO SetOperand OUT OF INDEX RANGE" << std::endl;
        std::cout << "m_instrTable[" << index+1 << "]" << std::endl;
    }
    m_table[index].operand = operand;
}

void CInstructionTable:: PrintError(const Error_InstrTable error,
                                    const CLexer& token) const
{
    long long colmNum = 0;
    if (token.GetColmNum() > 0)
    {
        colmNum = token.GetColmNum() - token.CalcErrorOffset();
    }
    
    std::cout << std::endl;
    
    switch(error)
    {
        case IT_MEM_OUT_OF_RANGE:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": invalid symbol table memory address" << std::endl;
            break;
            
        case IT_MAX_RANGE_REACHED:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": max symbol table memory reached" << std::endl;
            break;
            
        default:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": unknown instruction table error occured"
            << std::endl;
            break;
    }
    
}


void CInstructionTable::PrintTable() const
{
    //*** NEED TO FORMAT BETTER USING SETW
    if (PRINT_INSTRUCTION_TABLE)
    {
        if (0 != GetCurIndex())
        {
            for (int i = 0; i < GetCurIndex(); i++)
            {
                if ("NONE" != GetInstr(i))
                {
                    std::cout << GetAddress(i) << "\t\t"
                              << GetInstr(i)   << "\t\t";
                    if ("" != GetOperand(i))
                    {
                        std::cout << GetOperand(i);
                    }
                    std::cout << "\n";
                }
            }
        }
        else
        {
            std::cout << "The Instruction Table is Empty..." << std::endl;
        }
    }
}

int CInstructionTable::PopJumpStack()
{
    int topValue = m_jumpStack.top();
    m_jumpStack.pop();
    return topValue;
}

void CInstructionTable::PushJumpStack(const int addr)
{
    m_jumpStack.push(addr);
}


