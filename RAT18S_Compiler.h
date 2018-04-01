// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages

#ifndef RAT18S_Compiler_HEADER
#define RAT18S_Compiler_HEADER

#include <string>
#include <fstream>
#include "CLexer.h"
#include "CParser.h"

class RAT18S_Compiler
{
    
public:
//    RAT18S_Compiler();
//    RAT18S_Compiler(const RAT18S_Compiler& rhs) {*this = rhs;}
    
    // *** CHANGED THIS FUNCTION
    void Lexer() { m_lexer = m_lexer.Lexer();}
    
    // INPUT FILE FUNCTIONS
    void OpenFile(char* fileName) {m_lexer.OpenFile(fileName);}
    void OpenFile(std::string fileName) {m_lexer.OpenFile(fileName);}
    bool FileFail() {return m_lexer.FileFail();}
    bool FileGood() {return m_lexer.FileGood();}
    void CloseFile() {m_lexer.CloseFile();}
    
    // LEXER FUNCTIONS
    // *** ALL FUNCTIONS BELOW ADDED
    Token_Type GetTokenType() const {return m_lexer.GetTokenType();}
    std::string GetLexeme() const {return m_lexer.GetLexeme();}
    
    // CPARSER FUNCTIONS
    void Parse() {m_parser.Rat18S(m_lexer);}
    
private:
    // LEXER
    CLexer m_lexer;
    
    // PARSE
    CParser m_parser;
};

#endif    // END OF RAT18Scompiler_HEADER



