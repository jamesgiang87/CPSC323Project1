// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
//*** NEED TO BE ANSWERED QUESTIONS LEAD WITH ***

#include <iostream>		//not sure if needed if we will append all tokens to a list and return or print them here
#include <string>
#include <fstream>
#include <ctype>

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
	INSIDE_SEPARATOR,	// found {, (, or %

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

	//ERRORS FOR SYMBOLS
	INVALID_SYMBOL,		//Ex @, #, &

	//ERRORS FOR ERRORS HAHA
	UNKNOWN_ERROR
};

bool checkComments(const char curChar, STATES& curState);
bool checkIdentifier(const char curChar, STATES& curState);
bool checkKeyword(const std::string curToken, const STATES& curState);
bool checkNumber(const char curChar, STATES& curState);
bool checkOperators(const char curChar, STATES& curState);
bool checkSeparators(const char curChar, STATES& curState);
ERROR handleError(const STATES curState = UNKNOWN_ERROR);
void lexer(const std::ifstream& source); //this function should return a list of tokens
					 // or print them inside it

int main()
{
//	std::string myStr;
//	std::cout << "max string size is " << myStr.max_size() << std::endl;
// 	its size is 2^64 so i guess it depends also on operating system
	return 0;
}

//==============================================================================
// Description: This function will check and change the FSM (Finite State 
//		Machine) state if the current character from the source file is 
//		apart of a comment else it will handle the error.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state the FSM (Finite State Machine) is in.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool checkComments(const char curChar, STATES& curState)
{
	if(curChar == '!' && curState == INITIAL_STATE)
	{	
		// found a ! character which means beginning of a comment
		curState = INSIDE_COMMENT;
		return true;
	}	
	else if (curChar == '!' && curState == INSIDE_COMMENT)
	{
		// already found an initial ! character so this must be the end 
		//  the comment
		curState = END_COMMENT;
		return true;
		
	}
	else if (curChar == '!')
	{
		// an unmatch ! has been found
		ERROR error = handleError(curState);
		// ***NEED TO PRINT ERROR??
		return false;
	}

	return false;
}


//==============================================================================
// Description: This function will check and change the FSM (Finite State
//		Machine) state if the current character is apart of a legal
//		identifier else it will handle the error.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state the FSM (Finite State Machine) is in.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//==============================================================================
bool checkIdentifier(const char curChar, STATES& curState)
{
	//***IF YOU HAVE BETTER MORE SIMPLISTIC LOGIC PLEASE TELL ME
	
	if (isalpha(curChar))
	{
		if (curState == INITIAL_STATE || curState == INSIDE_IDENTIFIER 
	 	 || curState == COULD_END_IDENTIFIER))
		{
			// identifiers must lead with a letter or end with a
			// letter with the exception of ending with a $
			curState = COULD_END_IDENTIFIER;
			return true;
		}
		else
		{
			// found a letter in an illegal state
			ERROR error = handleError(curState);
			// ***NEED TO PRINT ERROR???
			return false;
		}
	}
	else if(char == '$')
	{  
		if (curState == INSIDE_IDENTIFIER 
		 || curState == COULD_END_IDENTIFIER))
		{
			// a $ cannot appear at the front of an identifier
			// only inside or at the end of an identifier
			curState = COULD_END_IDENTIFIER;
			return true;
		}
		else
		{
			// found a $ in an illegal state
			ERROR error = handleError(curState);
			// ***NEED TO PRINT ERROR???
			return false;
		}
	}
	elseif(isdigit(curChar) && (curState == INSIDE_IDENTIFIER 
				 || curState == COULD_END_IDENTIFIER))
	{
		// an identifier cannot end in a digit so it must still
		// be considered inside and identifier
		curState = INSIDE_IDENTIFIER;
		return true;

		// we do not through a error because if a digit is found
		// if may be apart of an integer or real number
	}
	
	return false;
}


//==============================================================================
//==============================================================================
bool checkKeyword(const std::string curToken, const STATES& curState)
{
	return false;
}


//==============================================================================
//==============================================================================
bool checkNumber(const char curChar, STATES& curState)
{
	return false;
}


//==============================================================================
//==============================================================================
bool checkOperators(const char curChar, STATES& curState)
{
	return false;
}


//==============================================================================
//==============================================================================
bool checkSeparators(const char curChar, STATES& curState)
{
	return false;
}


//==============================================================================
// Description: This function will determine an error given the current state
//		of the FSM (Finite State Machine). It will then return the
//		error as an enum ERROR type.
//
// Input: curState - the current state that the FSM (Finite State Machine) was
//		     at before the error occured.
//
// Output: ERROR - returns an enum error value to the corresponding error that
//		   occured.
//
//==============================================================================
ERROR handleError(const STATES curState)
{
	switch(curState)
	{
		// ERROR CASES FOR IDENTIFIERS
		case INSIDE_IDENTIFIER:
		case COULD_END_IDENTIFIER:
			return INVALID_IDENTIFIER;
		
		// ERROR CASES FOR NUMBERS
		case INSIDE_NUMBER:
			return INVALID_INT;

		case INSIDE_REAL:
		case COULD_END_REAL:
			return INVALID_REAL;

		// ERROR CASES FOR OPERATORS
		case POTENTIAL_OPERATOR:
			return INVALID_OPERATOR;


		// ERROR CASES FOR SEPARATORS
		case INSIDE_SEPARATOR:
			return UNCLOSED_SEPARATOR;


		// ERROR CASES FOR COMMENTS
		case INSIDE_COMMENT:
			return UNCLOSED_COMMENT;

		// ERROR CASE FOR SYMBOLS
		default:
			return INVALID_SYMBOL;
	}

	// ERROR CASES FOR THOSE YOU CANT THINK OF
	return UNKNOWN_ERROR;
}


//==============================================================================
//==============================================================================
void lexer(const std::ifstream& source)
{

}
