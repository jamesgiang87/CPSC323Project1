// Date: 4/17/2018
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
// File: CTables.cpp

#include "CTables.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>


//============================ CSymbolTable ====================================
CSymbolTable::CSymbolTable(): m_curIndex(0), m_curMemAddr(2000), m_error(NONE),
                              m_recVarTypeUsed(NO_TYPE), m_declaringVar(false)
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
        SetCurIndex(GetCurIndex()+1);
        SetCurMemAddr(GetCurMemAddr()+1);
    }
    else
    {
        // if we found a variable already in the table and we are declaring
        //  a variable instance then throw error
        if (IsDeclaringVar())
        {
            SetError(REDECLARATION);
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
    for (int i = 0; i < GetCurIndex(); i++)
    {
        if (varName == GetVarName(i))
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
        SetError(UNDECLARED);
        throw GetError();
    }
    
    //*** NOTE: SHOULD NEVER OCCUR
    return -1;
}


void CSymbolTable::SetVarTypeUsed(const VariableTypes varTypeUsed)
{
    m_recVarTypeUsed = varTypeUsed;
}


void CSymbolTable::SetVarName(const std::string varName, const int indexNum)
{
    //*** NOTE: SHOULD NEVER set m_varName to a variable name thats already
    //          in the table
    m_table[indexNum].m_varName = varName;
}


void CSymbolTable::SetMemAddr(const int memAddr, const int indexNum)
{
//    //*** NOTE: SHOULD NEVER set memAddr to a memory address less than 2000
//    if (memAddr >= 2000)
//    {
        m_table[indexNum].m_memAddr = memAddr;
//    }
//    else
//    {
//        SetError(MEM_OUT_OF_RANGE);
//        throw GetError();
//    }
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
    
    switch(error)
    {
        case REDECLARATION:
            std::cout  << "RAT18S error: " << token.GetLineNum() << ":"
            << colmNum << ": redeclaration of " << token.GetLexeme()
            << std::endl;
            break;
            
        case UNDECLARED:
        case MISMATCH:
        case MEM_OUT_OF_RANGE:
        case MAX_RANGE_REACHED:
        default:
            break;
    }
}


//============================ CInstructionTable ===============================
CInstructionTable::CInstructionTable() : m_curIndex(0)
{
    for (int i = 0; i < INSTRUCTION_TABLE_SIZE; i++)
    {
        SetAddress(0, i);
        SetInstr("NONE", i);
        SetOperand(0, i);
    }
}


void CInstructionTable::GenerateInstr(const InstructionType instType,
                                      const int addr)
{
    SetAddress(GetCurIndex()+1, GetCurIndex());
    SetInstr(GetInstrStr(instType), GetCurIndex());
    SetOperand(addr, GetCurIndex());
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


void CInstructionTable::SetOperand(const int operand, const int index)
{
    m_table[index].operand = operand;
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
                std::cout << GetAddress(i) << "\t\t"
                          << GetInstr(i)   << "\t\t"
                          << GetOperand(i) << "\n";
            }
        }
        else
        {
            std::cout << "The Instruction Table is Empty..." << std::endl;
        }
    }
}



