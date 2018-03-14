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
	CToken Lexer() { return m_lexer.Lexer(); }

	// INPUT FILE FUNCTIONS	
	void OpenFile(char* fileName) {m_lexer.OpenFile(fileName);}
	void OpenFile(std::string fileName) {m_lexer.OpenFile(fileName);}
	bool FileFail() {return m_lexer.FileFail();}
	bool FileGood() {return m_lexer.FileGood();}
	void CloseFile() {m_lexer.CloseFile();}

private:
	// LEXER
	CLexer m_lexer;
	
	// PARSE
	CParser m_parser;
};

#endif    // END OF RAT18Scompiler_HEADER

