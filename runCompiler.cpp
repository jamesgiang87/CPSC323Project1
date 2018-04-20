// Date: 2/17/18
// Class: CPSC 323 Compilers and Languages
// File: main.cpp

#include "RAT18S_Compiler.h"
#include "CLexer.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // create a RAT18S object
    RAT18S_Compiler compiler;
    
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
    
    compiler.Parse();
    compiler.PrintInstructionTable(); std::cout << "\n\n";
    compiler.PrintSymbolTable();
    
    
// *** TESTING ONLY
    std::cout << std::endl;
    std::cout << "PROGRAM COMPLETE" << std::endl;
    std::cout << std::endl;
// *** TESTING ONLNY
    
    std::cout << std::endl;
    
    // close the input file
    compiler.CloseFile();
    
    return 0;
}


