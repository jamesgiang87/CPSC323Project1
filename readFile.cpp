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




int main()
{
	
	ifstream inFile("C:\\Users\\4lph4fr34k\\Documents\\testfile.txt");
	const int dataSize = 100000;
	char data[dataSize];
	char temp;
	char exMark = ' ';
	char dQuoMark = ' ';
	int fileCount = 0;
	int print = 0;

	if (inFile.fail())
	{
		cerr << "file not found" << endl;
		exit(1);
	}

	else
	{

		
		while (!inFile.eof())
		{
			inFile.get(temp);
			
			if (temp == '!')
			{
				while (temp != '!')
				{
					inFile.get(temp);
					data[fileCount] = temp;
					fileCount++;
				}
				data[fileCount] = temp;
				fileCount++;
			}
			else
			{
			
				data[fileCount] = toupper(temp); 
				fileCount++;
			}
		}


		cout << fileCount << endl;

		while (print < fileCount)
		{

			cout << data[print];
			print++;

		}

		inFile.close();
	}

	system("pause");




}