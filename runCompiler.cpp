// Date: 2/17/18
// Class: CPSC 323 Compilers and Languages
// Description: This program will run a lexical analysis on the input file given


#include "RAT18Scompiler.h"
#include <iostream>
#include <fstream>

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
			return -1;	//error code, file not found
		}		
	}
	else	// if not input file given via cmd line... retrieve from user
	{
		std::string fileName;
		std::cout << "Please enter a filename: ";
		std::cin >> fileName;
		inputFile.open(fileName);

		if (inputFile.fail())
		{
			std::cout << "File " << fileName << " not found..." << std::endl;
			return -1;	// error code, file not found
		} 
	}

	
	RAT18Scompiler compiler;
	compiler.Lexer(inputFile);

// FOR TESTING ONLY
	std::cout << "COMPILATION COMPLETE..." << std::endl;


	outputFile.open("LexerAnalysis.txt");

	// close the output file
	outputFile.close();

	// close the input file
	inputFile.close();

	return 0;
}
