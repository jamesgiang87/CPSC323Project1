#include<iostream>
#include<fstream>
#include<ctype.h>
#include<string.h>

using namespace std;


// ============================================================================
// James Giang
// Date: 2/24/2018
// Description: This function read from file
// Char counter to count how big the array will be
// pending need to find lower case comments. Need to string with ! and make sure it has the same case.
//
// My test file right now is  james !billy bob! giang
// billy bob should be lower case
// ============================================================================

// ============================================================================
// Author: Erine Estrella
// Date: 2/15/2018
// Revisions:
//		- Fixed issue with comments being capitalized
//    - Made the lexer function the function that iterates through the file
//      instead of main
//    - Changed up the print function
//    - Added a display function but doesn't do anything yet!
// NOTES: I had to make a new filepath for the txt file that was being read in
//        for the program to work. I also changed the name of the array because
//        my debugger kept complaining how the name "data" for the array was
//        to ambiguious. 
// ============================================================================

std::ifstream testfile;
const int dataSize = 100000;
char filer[dataSize];
char temp;
char exMark = ' ';
char dQuoMark = ' ';
int fileCount = 0;
int print = 0;
int numOfComments = 0; //this will test program is picking up !

void Lexer(const std::ifstream& source); // this function should return a list 
										 //  of tokens or print them inside it
void display(); //will display all tokens 

void main()
{
	testfile.open("testfile.txt");
	

	if (testfile.fail())
	{
		cerr << "File not found" << endl;
		system("pause");
		exit(1);
	}

	else
	{
		Lexer(testfile);
		cout << "Filecount : " << fileCount << endl;
		cout << "Number of comments found : " << numOfComments << endl;
		cout << "File contents : ";
		while (print < fileCount)
		{
			cout << filer[print];
			print++;
		}
		testfile.close();
	}
	cout << endl;
	system("pause");

}


void Lexer(const std::ifstream& source)
{
	while (!testfile.eof())
	{
		testfile.get(temp);

		if (temp == '!')
		{
			filer[fileCount] = temp;
			fileCount++;
			testfile.get(temp);
			testfile.unget();
			while (temp != '!')
			{
				testfile.get(temp);
				filer[fileCount] = temp;
				fileCount++;
			}
			numOfComments++;
		}
		else
		{
			filer[fileCount] = toupper(temp);
			fileCount++;
		}
	}
}

void display()
{
	cout << "token\t\t\t\t lexeme\n";
	cout << "____________________________\n";
}
