// Date: 2/17/18
// Class: CPSC 323 Compilers and Languages
// Description: This program will run a lexical analysis on the input file given


#include "RAT18S_Compiler.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void OutputToken(std::ofstream& outputFile, const TOKEN token);

int main(int argc, char* argv[])
{
    
    // stores the input file
    std::ifstream inputFile;
    
    // stores the output file
    std::ofstream outputFile;
    
    // check if input file given via cmd line (0 = filename, 1 = input file)
    if (argc == 2)
    {
        //if we recieved one argument from command line that is the file
        inputFile.open(argv[1]);
        
        if (inputFile.fail())
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
        inputFile.open(fileName);
        
        if (inputFile.fail())
        {
            std::cout << "File " << fileName << " not found..." << std::endl;
            return -1;    // error code, file not found
        }
    }
    
    // create output file to store output to
    outputFile.open("LexerAnalysis.txt");
    outputFile << "Token\t\t\t\t\tLexeme\n";
    outputFile << "----------------------------------------------\n";
    
    // create a RAT18S object
    RAT18S_Compiler compiler;
    
    // stores the tokens returned from the lexer
    TOKEN token;
    
    while(inputFile.good())
    {
        token = compiler.Lexer(inputFile);
        
        if (!token.lexeme.empty())
        {
            OutputToken(outputFile, token);
            std::cout << "token found was " << token.lexeme << "\n";
        }
        
        // reset the token type and clear lexeme
        token.tokenType = TYPE_ERROR;
        token.lexeme.clear();
    }
    
    // close the output file
    outputFile.close();
    
    // close the input file
    inputFile.close();
    
    return 0;
}


void OutputToken(std::ofstream& outputFile, const TOKEN token)
{
    switch(token.tokenType)
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
            
        case COMMENT:
            outputFile << std::left << std::setw(strlen("identifier"));
            outputFile << "comment";
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
    
    int extraSpacing = strlen("identifier") - token.lexeme.length();
    outputFile << std::setw(25+extraSpacing) << " " << token.lexeme << "\n";
}

