//Date: 3/14/2018
// Author: Austin Blanke
// Class: CPSC 323 Compilers and Languages

#include "RAT18S_Compiler.h"
#include <iomanip>

//=============================================================================
// Description: This function will print if the token along with its lexeme.
// Input: outputFile - the output file we want to print the token and lexeme
//		       values to.
// Output: NONE 
//=============================================================================
void RAT18S_Compiler::OutputToken(std::ofstream& outputFile)
{
    switch(m_lexer.GetTokenType())
    {
        case IDENTIFIER:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile <<  "identifier";
            break;
            
        case KEYWORD:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "keyword";
            break;
        case INT:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "int";
            break;
            
        case REAL:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "real";
            break;
            
        case OPERATOR:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "operator";
            break;
            
        case SEPARATOR:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "separator";
            break;
            
        case SYMBOL:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "symbol";
            break;
            
        default:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "unknown";
            break;
            
    }
    
    int extraSpacing = strlen("identifier") - m_lexer.GetLexemeLength();
    outputFile << std::setw(25+extraSpacing) << " " << m_lexer.GetLexeme() << "\n";
}
