// Date: 2/17/18
// Class: CPSC 323 Compilers and Languages
// Description: This program will run a lexical analysis on the input file given



#include "RAT18S_Compiler.h"
#include "CLexer.h"
#include <iostream>
#include <iomanip>

void OutputCToken(std::ofstream& outputFile, const CToken token);
void PrintOutputHeader(std::ofstream& outputFile);

int main(int argc, char* argv[])
{
    // create a RAT18S object
    RAT18S_Compiler compiler;
    
    // stores the output file
    std::ofstream outputFile;
    
    // check if input file given via cmd line (0 = filename, 1 = input file)
    if (argc == 2)
    {
        //if we recieved one argument from command line that is the file
        compiler.OpenFile(argv[1]);
        
        if (compiler.FileFail())
        {
            std::cout << "File " << argv[1] << " not found..." << std::endl;
            return -1;    //error code, file not found
        }
    }
    else    // if not input file given via cmd line... retrieve from user
    {
        std::string fileName;
        std::cout << "Please enter a filename: ";
        std::cin >> fileName;
        compiler.OpenFile(fileName);
        
        if (compiler.FileFail())
        {
            std::cout << "File " << fileName << " not found..." << std::endl;
            return -1;    // error code, file not found
        }
    }
 
    PrintOutputHeader(outputFile);   
        
    // stores the tokens returned from the lexer
    CToken token;
    
    while(compiler.FileGood())
    {
        token = compiler.Lexer();
        
        if (!token.IsLexemeEmpty())
        {
            OutputCToken(outputFile, token);
        }
        
        // reset the token type and clear lexeme
        token.SetTokenType(TYPE_ERROR);
        token.ClearLexeme();
    }
    
    // close the output file
    outputFile.close();
    
    // close the input file
    compiler.CloseFile();
    
    return 0;
}


void PrintOutputHeader(std::ofstream& outputFile)
{
    // create output file to store output to
    outputFile.open("LexerAnalysis.txt");
    outputFile << "Token\t\t\t\t\tLexeme\n";
    outputFile << "----------------------------------------------\n";
}



void OutputCToken(std::ofstream& outputFile, const CToken token)
{
    switch(token.GetTokenType())
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
    
    int extraSpacing = strlen("identifier") - token.GetLexemeLength();
    outputFile << std::setw(25+extraSpacing) << " " << token.GetLexeme() << "\n";
}

