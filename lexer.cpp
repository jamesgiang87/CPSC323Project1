// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages

#include <iostream>		//not sure if needed if we will append all tokens to a list and return or print them here
#include <string>

//only store accepting states of our FSM (Finite State Machine)
enum ACCEPTING_STATES
{
	//STATES FOR IDENTIFIERS
	END_IDENTIFIER,

	//STATES FOR NUMBERS
	END_INT,
	END_REAL,
	
	//STATES FOR KEYWORDS
	END_KEYWORD,

	//STATES FOR OPERATORS
	END_OPERATOR,
	
	//STATES FOR SEPARATORS
	END_SEPERATOR,
	
	//STATES FOR COMMENTS
	END_COMMENT
};


//stores every other state of our FSM (Finite State Machine)
enum STATES
{
	INITIAL_STATE,		// first state of the FSM

	//STATES FOR IDENTIFIERS
	INSIDE_IDENTIFIER,	// only when it sees a digit after letter
	COULD_END_IDENTIFIER,	// only last letter, $ or letter in the middle

	//STATES FOR NUMBERS
	INSIDE_NUMBER,		// when a digit is found and current state is initial
	INSIDE_REAL,		// when a '.' is found after a digit is
	COULD_END_REAL,		// when a digit is found after a '.'

	//STATES FOR OPERATORS
	POTENTIAL_OPERATOR,

	//STATES FOR SEPARATORS
	INSIDE_SEPERATOR,	// found {, (, or %

	//STATES FOR COMMENTS
	INSIDE_COMMENT		// found initial !
};


//stores every error flag that may occur in our FSM (Finite State Machine)
enum ERROR
{
	//ERRORS FOR IDENTIFIERS
	INVALID_IDENTIFIER,	// Ex a8, a$2	

	//ERRORS FOR NUMBERS
	INVALID_INT,		// Ex 8a
	INVALID_REAL,		// Ex 7.7. .9

	//ERRORS FOR OPERATORS
	INVALID_OPERATOR,	// Ex **, ++, --

	//ERRORS FOR SEPARATORS
	UNCLOSED_SEPARATOR,	//Ex {only, ( only, (()

	//ERRORS FOR COMMENTS
	UNCLOSED_COMMENT,	//Ex !only, !!!

	//ERROS FOR SYMBOLS
	INVALID_SYMBOL		//Ex @, #, &
};

bool checkComments(const char curChar, STATES& curState);
bool checkIdentifier(const char curChar, STATES& curState);
bool checkKeyword(const std::string curToken, const STATES& curState);
bool checkNumber(const char curChar, STATES& curState);
bool checkOperators(const char curChar, STATES& curState);
bool checkSeparators(const char curChar, STATES& curState);
STATES handleError(const STATES curState);

int main()
{
//	std::string myStr;
//	std::cout << "max string size is " << myStr.max_size() << std::endl;
// 	its size is 2^64 so i guess it depends also on operating system
	return 0;
}


bool checkComments(const char curChar, STATES& curState);
bool checkIdentifier(const char curChar, STATES& curState);
bool checkKeyword(const std::string curToken, const STATES& curState);
bool checkNumber(const char curChar, STATES& curState);
bool checkOperators(const char curChar, STATES& curState);
bool checkSeparators(const char curChar, STATES& curState);
STATES handleError(const STATES curState);
