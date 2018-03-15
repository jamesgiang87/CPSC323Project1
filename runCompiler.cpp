// Date: 2/17/18
// Class: CPSC 323 Compilers and Languages
// Description: This program will run a lexical analysis on the input file given



#include "RAT18S_Compiler.h"
#include "CLexer.h"
#include <iostream>

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
 
    // create output file to store output to
    outputFile.open("LexerAnalysis.txt");
    
    // stores the tokens returned from the lexer
    CToken token;
    
    while(compiler.FileGood())
    {
        token = compiler.Lexer();
        
        if (!token.IsLexemeEmpty())
        {
            compiler.OutputToken(outputFile);
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

