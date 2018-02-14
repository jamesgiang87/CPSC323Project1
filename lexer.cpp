// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
//*** NEED TO BE ANSWERED QUESTIONS LEAD WITH ***

// !!! NOTES !!!
// !!! CIN AUTOMATICALLY EXTRACTS ALL CHARACTERS EXCLUDING WHITESPACE BUT IVE BEEN MANAGING THIS PROBLEM BY 
// !!!  EXTRACTING ONE CHARACTER AT A TIME SO IM NOT SURE MY ALGORITHM/MACHINE WOULD WORK
// 
// **** NEED TO DO ****
// *** ADD FUNCTIONALITY TO MOVE FILE POINTER BACKWARDS BEFORE ADDING CHARACTER TO TOKEN (can only happen
//	with whitespace, operators and separators)
// *** ADD LINE NUMBER AND COLUMN NUMBER VARIABLES TO KEEP TRACK IN LEXER()
// *** CAPATALIZE ALL CHARACTERS IN FILE EXCEPT COMMENTS OR ""
// *** PLEASE CHECK FSM LOGIC!!!


#include <cctype>
#include <fstream>
#include <iostream>		
#include <string>

 
// stores all keywords available in RAT18S
// THIS MUST BE SORTED!!!
const std::string KEYWORDS[KEYWORDS_AMOUNT] = {"BOOLEAN", "ELSE", "ENDIF", "FALSE", 
					       "GET", "IF", "INT", "PUT", "REAL", 
					       "RETURN", "TRUE", "WHILE"}; 

// stores all separators available in RAT18S
const char SEPARATORS[SEPARATOR_AMOUNT] = {';' , '(' , ')' , '{' , '}' , '%' };

// stores all legal whitespace characters available in RAT18S
const char WHITESPACES[WHITESPACE_AMOUNT] = {' ', '\t', '\n'};

// stores every other state of our FSM (Finite State Machine)
enum STATES
{
	INITIAL_STATE,		// first state of the FSM

	//STATES FOR IDENTIFIERS
	INSIDE_IDENTIFIER,	// only when it sees a digit after letter
	COULD_END_IDENTIFIER,	// only last letter, or letter in the middle
	DOLLAR_IDENTIFIER,	// only last character can be $ if its found
	END_IDENTIFIER,		// AN ACCEPTING STATE IDENTIFIERS
	
	//STATES FOR NUMBERS
	INSIDE_NUMBER,		// a digit is found and current state is initial
	END_INT,		// AN ACCEPTING STATE FOR INTEGERS
	INSIDE_REAL,		// when a '.' is found after a digit is
	COULD_END_REAL,		// when a digit is found after a '.'
	END_REAL,		// AN ACCEPTING STATE FOR REALS

	//STATES FOR KEYWORDS
	END_KEYWORD,		// AN ACCEPTING STATE FOR KEYWORDS

	//STATES FOR OPERATORS
	CARROT_OPERATOR,	// found an initial ^
	EQUAL_OPERATOR,		// found =
	POTENTIAL_OPERATOR,	// found +, -, /, *, >, <
	END_OPERATOR,		// AN ACCEPTING STATE FOR OPERATORS

	//STATES FOR SEPARATORS
	INSIDE_SEPARATOR,	// found {, (
	PERCENT_SEPARATOR,	// found %
	END_SEPARATOR,		// AN ACCEPTING STATE FOR SEPARATORS

	//STATES FOR COMMENTS
	INSIDE_COMMENT,		// found initial !
	END_COMMENT		// AN ACCEPTING STATE FOR COMMENTS
};


// stores every error flag that may occur in our FSM (Finite State Machine)
enum ERROR
{
	//ERRORS FOR IDENTIFIERS
	INVALID_IDENTIFIER,	// Ex a8, a$2	

	//ERRORS FOR NUMBERS
	INVALID_INT,		// Ex 8a
	INVALID_REAL,		// Ex 7.7. .9

	//ERRORS FOR OPERATORS
	INVALID_OPERATOR,	// Ex **, ++, --

	//ERRORS FOR SYMBOLS
	INVALID_SYMBOL,		//Ex @, #, &

	//ERRORS FOR ERRORS HAHA
	UNKNOWN_ERROR
};


// stores the different types a token can be
enum TOKEN_TYPE
{
	IDENTIFIER,
	INT,
	REAL,
	OPERATOR,
	SEPARATOR,
	COMMENT,
	SYMBOL,
	ERROR		// *** NOT SURE IF THIS IS NEEDED
};


struct TOKEN
{
	TOKEN_TYPE tokenType;	// store the type of token was found (ex. Real, Int, Separator)
	std::string lexeme;	// store the actual instance of the token
};


const int KEYWORDS_AMOUNT   = 12;
const int SEPARATOR_AMOUNT  = 6;
const int WHITESPACE_AMOUNT = 3;

bool CheckBackup(const char curChar, const STATES& curState);
bool CheckComments(const char curChar, STATES& curState);
bool CheckIdentifier(const char curChar, STATES& curState);
bool CheckKeyword(const std::string curToken, STATES& curState);
bool CheckNumber(const char curChar, STATES& curState);
bool CheckOperators(const char curChar, STATES& curState);
bool CheckSeparators(const char curChar, STATES& curState);
ERROR DetermineError(const STATES curState);
void Lexer(const std::ifstream& source); // this function should return a list 
					 //  of tokens or print them inside it
bool IsAcceptingState(const STATES curState);
bool IsSeparator(const char curChar);
bool IsWhiteSpace(const char curChar);
void PrintError(const int lineNum, const int colmNum, const std::string token, const ERROR errorType);



//==============================================================================
// Description: This function will check the given character recieved from the
//		source file and compare it with the current FSM (Finite State
//		Machine) and change the state if it determines it needs to back 
//		up the current file pointer. If the current file pointer needs 
//		to be backed up this function will back it up. If the FSM
//		(Finite State Machine) is will throw an ERROR type.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state the FSM (Finite State Machine) is in.
//
// Output: true - the file pointer was backed up. 
//	   false - the file pointer was not backed up.
//
//==============================================================================
// ***NOT SURE IF THIS ACTUALLY NEEDS TO BE BOOL
bool CheckBackup(const char curChar, STATES& curState)
{// ***POTENTIALLY UNFINISHED

	bool pushBack = false;		// determines if file pointer needs
					//  to be pushed back

	if (isWhiteSpace(curChar) || isSeparator(curChar) || isOperator(curChar))
	{
		switch(curState)
		{

			// cases for IDENTIFIERS
			case COULD_END_IDENTIFIER:
			case DOLLAR_IDENTIFIER:
				// we have just found out that it is an
				//  identifier so label as accepted
				curState = END_IDENTIFIER;
				pushBack = true;
				break;

			// cases for NUMBERS
			case INSIDE_NUMBER:
				// we have just found out that its is not a
				//  number anymore so we label it as accepted
				curState = END_INT;
				pushBack = true;
				break;

			case COULD_END_REAL:
				// we just found out that it is a real number
				//  so we label it as an accepting state
				curState = END_REAL;
				pushBack = true;
				break;
		
			// cases for OPERATORS
				// not all operators need to be here
			case POTENTIAL_OPERATOR:
			case EQUAL_OPERATOR:
				
				// we have just found a > or < and IT MUST BE A SPACE
				if (!isOperator(curChar))
				{
					curState = END_OPERATOR;
					pushBack = true;
				}
				else
				{
					throw determineError(curState);
				}
				
				break;
				
			// dont need it for separatros or comments because we
			//  know when we find the second one it is accepted
			default:
				// we need to throw error
				throw handleError(curState);
		}

		if (pushBack)
		{
			// ***NEED TO PUSHBACK THE FILE POINTER
		}
	}

	return false;
}



//==============================================================================
// Description: This function will check and change the FSM (Finite State 
//		Machine) state if the current character from the source file is 
//		apart of a comment else it will throw an ERROR type.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state the FSM (Finite State Machine) is in.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool CheckComments(const char curChar, STATES& curState)
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
		throw handleError(curState);
	}

	return false;
}


//==============================================================================
// Description: This function will check and change the FSM (Finite State
//		Machine) state if the current character is apart of a legal
//		identifier else it will throw an ERROR type.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state the FSM (Finite State Machine) is in.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool CheckIdentifier(const char curChar, STATES& curState)
{
	if (std::isalpha(curChar) && (curState == INITIAL_STATE 
	  || curState == INSIDE_IDENTIFIER || curState == COULD_END_IDENTIFIER))
	{
		// identifiers must lead with a letter or end with a
		// letter with the exception of ending with a $
		curState = COULD_END_IDENTIFIER;
		return true;
		
	}
	else if(curChar == '$' && curState == COULD_END_IDENTIFIER)
	{  
		// a $ cannot appear at the front of an identifier
		// only inside or at the end of an identifier
		curState = DOLLAR_IDENTIFIER;
		return true;
	}
	else if(std::isdigit(curChar) && (curState == INSIDE_IDENTIFIER 
				 || curState == COULD_END_IDENTIFIER))
	{
		// an identifier cannot end in a digit so it must still
		// be considered inside and identifier
		curState = INSIDE_IDENTIFIER;
		return true;

		// we do not throw an error because if a digit is found
		// if may be apart of an integer or real number
	}
	else
	{
		//found an $ or letter in an illegal state
		throw handleError(curState);
	}
	
	return false;
}


//==============================================================================
// Description: This function will determine if a given token is a keyword or
//		not. If the token is a keyword the current State of the FSM
//		(Finite State Machine) will be updated accordingly.
//
// Input: curToken - the current token we want to determine is a keyword or not.
//	  curState - the current state of the FSM (Finite State Machine).
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//	   false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool CheckKeyword(const std::string curToken, STATES& curState)
{
	// highest index of the list we are searching
	int maxIndex = KEYWORDS_AMOUNT;	
	
	// lowest index of the list we are searching
	int minIndex = 0;	
	
	// stores where we are current looking in the keyword list
	int curIndex = minIndex + (maxIndex - minIndex)/2;	
	
	do
	{
		if (curToken == KEYWORDS[curIndex])
		{
			curState = END_KEYWORD;
			return true;
		}
		else if (curToken < KEYWORDS[curIndex])
		{
			maxIndex = curIndex - 1;
			curIndex = minIndex + (maxIndex - minIndex)/2; 
		}
		else //curToken > KEYWORDS[curIndex]
		{
			maxIndex = curIndex - 1;
			curIndex = minIndex + (maxIndex - minIndex)/2;
		}
	}while(minIndex <= maxIndex);
	
	return false;
}


//==============================================================================
// Description: This function will check and change the FSM (Finite State 
//		Machine) state if the current character is apart of a legal 
//		number, such as an integer or real, else it will issue an error.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state of the FSM (Finite State Machine)
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//	   false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool CheckNumber(const char curChar, STATES& curState)
{
	bool isDigit = std::isdigit(curChar);

	if (isDigit && curState == INITIAL_STATE)
	{
		// found a number so we are inside a number
		curState = INSIDE_NUMBER;
		return true;
	}
	else if (isDigit && curState == INSIDE_NUMBER)
	{
		// if isdigit and its inside a number do NOTHING
		//state really hasnt changed
		return false;
	}
	else if (curChar == '.')
	{
		if (curState == INSIDE_NUMBER)
		{
			curState = INSIDE_REAL;
			return true;	
		}
		else
		{
			throw handleError(curState);
		}
	}
	else if (isDigit && (curState == INSIDE_REAL || curState == COULD_END_REAL))
	{
		// a digit followed after the '.' and possibly more digits found
		curState = COULD_END_REAL;
		return true;
	}
	
	return false;
}


//==============================================================================
// Description: This function will check and change the FSM (Finite State 
//		Machine) state if the current character is apart of a legal 
//		operator else it will issue an error.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state of the FSM (Finite State Machine)
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//	   false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool CheckOperators(const char curChar, STATES& curState)
{
	switch(curChar)
	{
		// symbols could be added to these operators or we are looking
		//  for a space to complete it
		case '+':
		case '-':
		case '*':
		case '/':
			if (curState == INITIAL_STATE)
			{
				curState = END_OPERATOR;
				return true;
			}
			else
			{	// invalid symbol found in wrong state
				throw handleError(curState);
			}

			break;

		case '^':
			if (curState == INITIAL_STATE)
			{
				// found the first part of ^= operator
				// need special enum because we cant have ^>
				// or ^space as an operator
				curState = CARROT_OPERATOR;
				return true;
			}
			else
			{
				// invalid symbol found in wrong state
				throw handleError(curState);
			}

			break;

		case '>':
		case '<':	
			if (curState == INITIAL_STATE)
			{
				// found either <, or > operator
				curState = POTENTIAL_OPERATOR;
				return true;
			}
			else if (curState == EQUAL_OPERATOR)
			{	
				// found portion of =>, or =< operator
				curState = POTENTIAL_OPERATOR;
				return true;
			}
			else
			{
				// invalid symbol found in wrong state
				throw handleError(curState);
			}

			break;

		case '=':	
			if (curState == INITIAL_STATE)
			{
				// found first portion of ==, =>, or =<
				curState = EQUAL_OPERATOR;
				return true;
			}
			else if (curState == CARROT_OPERATOR)
			{
				// found the ^= operator
				curState = END_OPERATOR;
				return true;
			}
			else if (curState == EQUAL_OPERATOR)
			{
				// found the == operator
				curState = END_OPERATOR;
				return true;
			}
			else
			{
				// invalid symbol found in wrong state
				throw handleError(curState);
			}

			break;

		default:
			throw handleError(curState);
	}
	
	return false;
}


//==============================================================================
// Description: This function will check and change the FSM (Finite State
//		Machine) state if the current character is apart of a legal
//		separator else it will issue an error.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state the FSM (Finite State Machine) is in.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool CheckSeparators(const char curChar, STATES& curState)
{
	if (isSeparator(curChar) && curState == INITIAL_STATE)
	{
		// found the first part of a separator character
		if (curChar == '%')
		{
			//special case as %% is a separator not {% or (% etc.
			curState = PERCENT_SEPARATOR;
			return true;
		}
		else
		{
			curState = INSIDE_SEPARATOR;
			return true;
		}
	}
	else if ((isSeparator(curChar) && curState == INSIDE_SEPARATOR) ||
		 (curChar == '%'&& curState == PERCENT_SEPARATOR))
	{
		// found the closing part of a separator character
		curState = END_SEPARATOR;
		return true;
	}
	else
	{
		// separator was found in an invalid state
		throw handleError(curState);
	}

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
ERROR DetermineError(const STATES curState)
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
void Lexer(const std::ifstream& source)
{

}


//==============================================================================
// Description: This function will determine if the current state of the FSM
//		(Finite State Machine) is in an accepting state or not. If the
//		FSM (Finite State Machine) is in an accepting state it will
//		return true or false. 
// 
// Input: curState - the current state of the FSM (Finite State Machine).
//
// Output: true - the current state of the FSM (Finite State Machine) is in an
//		  accepting state.
//	   false - the current state of the FSM (Finite State Machine) is not
//		   in an accepting state.
//
//==============================================================================
bool IsAcceptingState(const STATES curState)
{
	switch(curState)
	{
		case END_IDENTIFIER:
		case END_INT:
		case END_REAL:
		case END_KEYWORD:
		case END_OPERATOR:
		case END_SEPARATOR:
		case END_COMMENT:
			return true;	// an accepting state
		default:
			return false;	// not an accepting state
	}	

	return false;
}

//==============================================================================
// Description: This function will determine if the char passed by argument is
//		a legal separator in the RAT18S language.
// 
// Input: curChar - the current char we are testing to determine if its a 
//		    legal separator character.
//
// Output: true - curChar is a legal separator character for RAT18S.
//	   false - curChar is an illegal separator character for RAT18S.
//
//==============================================================================
bool IsSeparator(const char curChar)
{
	for (int i = 0; i < SEPARATOR_AMOUNT; i++)
	{
		if (curChar == SEPARATORS[i])	return true;
	}

	return false;
}


//==============================================================================
// Description: This function will determine if the char passed by argument is
//              a legal whitespace character in the RAT18S language.
//
// Input: curChar - the current char we are testing to determine if its a 
//		    legal whitespace character.
//
// Output: true - curChar is a legal whitespace character for RAT18S.
//	   false - curChar is an illegal whitespace character for RAT18S.
//
//==============================================================================
bool IsWhiteSpace(const char curChar)
{
	for (int i = 0; i < WHITESPACE_AMOUNT; i++)
	{
		if (curChar == WHITESPACES[i])	return true;
	}

	return false;
}



//==============================================================================
// Description: This function will print out a given Error along with the 
//		possible token we believe is in error as well as what line 
//		number and column number this error can be found.
//
// Input: lineNum   - the line the error was found on
//	  colmNum   - the column the error was found on
//	  token     - the token we believe is in error
//	  errorType - the type of error that was caused
//
// Output: NONE
//
//==============================================================================
void PrintError(const int lineNum, const int colmNum, const std::string token, 
		const ERROR errorType)
{

	std::cout << lineNum << ":" << colmNum << " error: ";

	switch(errorType)
	{
		case INVALID_IDENTIFIER:
			std::cout << " invalid identifier." << std::endl;
			break;
					
		case INVALID_INT:
			std::cout << " invalid integer." << std::endl;
			break;
				
		case INVALID_REAL:
			std::cout << " invalid real." << std::endl;
			break;
			
		case INVALID_OPERATOR:
			std::cout << " invalid operator." << std::endl;
			break;

		case INVALID_SYMBOL:
			std::cout << " invalid symbol." << std::endl;
			break;
	
		default:
			 // unknown error
			std::cout << " unknown error occured." << std::endl;
			break;
	}
}



