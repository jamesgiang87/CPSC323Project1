// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
//*** NEED TO BE ANSWERED QUESTIONS LEAD WITH ***

/// **** MAJOR PROBLEM *****
// *** ADDED STACK TO STORE FSM STATE SO WHEN WE SEE SEPARATORS LIKE
//		({identifier1$ == 25})
//     WE KNOW WE HAVE BALANCED BRACES 
// *** NEED STRUCTURE TO RETURN TOKEN WITH LEXEMES
// print them here
// **** CAN WE USE VECTOR LIKE AN ARRAY, dynamic array??
// *** DO WE NEED TO LOOK FOR UNMATCHING SEPARATORS???

// **** NEED TO DO ****
// *** ADD FUNCTIONALITY TO MOVE FILE POINTER BACKWARDS BEFORE ADDING CHARACTER TO TOKEN (can only happen with whitespace,
//	operators and separators)
// *** NEED TO REMOVE STACK BALANCED BRACES
// *** COMBINE ALL SEPERATOR ARRAYS TOGETHER (DELETE EXTRA FUNCTIONS FOR THAT)
// *** CHECK IDENTIFIER FSM LOGIC
// *** NEED TO REMOVE isWhiteSpace() functions from FSMs
// *** CAPATALIZE ALL CHARACTERS IN FILE EXCEPT COMMENTS OR ""
// *** NEED TO ADD STURCTURE TO RETURN TOKEN ALONG WITH LEXEMES
// *** NEED TO KEEP TRACK OF LINE NUMBER AND COLUMN NUMBER... WHEN ERROR ACCURS PRINT LINE NUMBER AND COLUMN #
//     ALONG WITH THE TOKEN THAT WAS MESSED UP IF ITS NOT A MISSING SEPARATOR OR COMMENT

#include <cctype>
#include <fstream>
#include <iostream>		
#include <stack>
#include <string>

const int KEYWORDS_AMOUNT = 12;
const int OPEN_SEPARATOR_AMOUNT = 3;
const int CLOSED_SEPARATOR_AMOUNT = 4; 
const int WHITESPACE_AMOUNT = 3;
 
// stores all keywords available in RAT18S
// THIS MUST BE SORTED!!!
const std::string KEYWORDS[KEYWORDS_AMOUNT] = {"BOOLEAN", "ELSE", "ENDIF", "FALSE", 
					       "GET", "IF", "INT", "PUT", "REAL", 
					       "RETURN", "TRUE", "WHILE"}; 

// stores all separators available in RAT18S
const char OPEN_SEPARATORS[OPEN_SEPARATOR_AMOUNT] = {'(' , '{' , '%' };

// stores all closing separators available in RAT18S
const char CLOSED_SEPARATORS[CLOSED_SEPARATOR_AMOUNT] = {';' , ')' , '}' , '%'};

// stores all legal whitespace characters available in RAT18S
const char WHITESPACES[WHITESPACE_AMOUNT] = {' ', '\t', '\n'};

// stores every other state of our FSM (Finite State Machine)
enum STATES
{
	INITIAL_STATE,		// first state of the FSM

	//STATES FOR IDENTIFIERS
	INSIDE_IDENTIFIER,	// only when it sees a digit after letter
	COULD_END_IDENTIFIER,	// only last letter, $ or letter in the middle
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

	//ERRORS FOR SEPARATORS
	UNCLOSED_SEPARATOR,	//Ex {only, ( only, ((), (%, {%, [

	//ERRORS FOR COMMENTS
	UNCLOSED_COMMENT,	//Ex !only, !!!

	//ERRORS FOR SYMBOLS
	INVALID_SYMBOL,		//Ex @, #, &

	//ERRORS FOR ERRORS HAHA
	UNKNOWN_ERROR
};


// stores all states that need to be saved
std::stack<STATES> STATES_STACK;


bool checkComments(const char curChar, STATES& curState);
bool checkIdentifier(const char curChar, STATES& curState);
bool checkKeyword(const std::string curToken, STATES& curState);
bool checkNumber(const char curChar, STATES& curState);
bool checkOperators(const char curChar, STATES& curState);
bool checkSeparators(const char curChar, STATES& curState);
ERROR handleError(const STATES curState);
void lexer(const std::ifstream& source); // this function should return a list 
					 //  of tokens or print them inside it
bool isAcceptingState(const STATES curState);
bool isOpenSeparator(const char curChar);
bool isClosedSeparator(const char curChar);
bool isWhiteSpace(const char curChar);



int main()
{
//	std::string myStr;
//	std::cout << "max string size is " << myStr.max_size() << std::endl;
// 	its size is 2^64 so i guess it depends also on operating system
/*	std::stack<int> myStack;
	myStack.push(10);
	myStack.push(20);
	myStack.push(30);
	std::cout << "Stack size is " << myStack.size() << std::endl;
	myStack.pop();
	myStack.pop();
	myStack.pop();
	std::cout << "Stack size is " << myStack.size() << std::endl;
*/	return 0;
}

//==============================================================================
// Description: This function will check and change the FSM (Finite State 
//		Machine) state if the current character from the source file is 
//		apart of a comment else it will issue an error.
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
//		identifier else it will issue an error.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//	  curState - the current state the FSM (Finite State Machine) is in.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool checkIdentifier(const char curChar, STATES& curState)
{
	//***IF YOU HAVE BETTER MORE SIMPLISTIC LOGIC PLEASE TELL ME
	
	if (std::isalpha(curChar))
	{
		if (curState == INITIAL_STATE || curState == INSIDE_IDENTIFIER 
	 	 || curState == COULD_END_IDENTIFIER)
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
	else if(curChar == '$')
	{  
		if (curState == COULD_END_IDENTIFIER)
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
	else if(isWhiteSpace(curChar) && curState == COULD_END_IDENTIFIER)
	{
		curState = END_IDENTIFIER;
		return true;
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
bool checkKeyword(const std::string curToken, STATES& curState)
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
bool checkNumber(const char curChar, STATES& curState)
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
			ERROR error = handleError(curState);
			// ***NEED TO PRINT ERROR?
			return false;
		}
	}
	else if (isDigit && (curState == INSIDE_REAL || curState == COULD_END_REAL))
	{
		// a digit followed after the '.' and possibly more digits found
		curState = COULD_END_REAL;
		return true;
	}
	else if (isWhiteSpace(curChar) || isOpenSeparator(curChar) || NEED TO ADD OPERATORS HERE)
	{
		if (curState == INSIDE_NUMBER)
		{
			//if all has been found is numbers then its an integer
			curState = END_INT;
			return true;
		}
		else if (curState == COULD_END_REAL)
		{
			//digit then '.' then digit has been found
			curState = END_REAL;
			return true;
		}
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
bool checkOperators(const char curChar, STATES& curState)
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
				ERROR error = handleError(curState);
				// ***NEED TO PRINT ERROR???
				return false;	
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
				ERROR error = handleError(curState);
				// ***NEED TO PRINT ERROR???
				return false;
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
				ERROR error = handleError(curState);
				// ***NEED TO PRINT ERROR???
				return false;
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
				ERROR error = handleError(curState);
				// ***NEED TO PRINT ERROR???
				return false;
			}

			break;

		case ' ':
		case '\t':
		case '\n':
			// state only become END_OPERATOR when it sees 
			//  whitespace after the operator.. so that we dont
			//  have === instances and this could happend because
			//  it read the first two == and says END_OPERATOR
			//  but the next char is = therefore making it illegal
			if (curState == POTENTIAL_OPERATOR 
			 || curState == EQUAL_OPERATOR)
			{
				// need this case for +, -, /, *, =, >, or <
				curState = END_OPERATOR;
				return true;
			}
		 	else if (curState == END_OPERATOR)
			{	// should never execute	
				// nothing to do already know its in an 
				//  accepting state
				return false;
			}
		
			break;
		
		default:
			ERROR error = handleError(curState);
			return false;
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
bool checkSeparators(const char curChar, STATES& curState)
{
	// look at past FSM (Finite State Machine) states to determine next step
	if (!STATES_STACK.empty())
	{
		curState = STATES_STACK.top();
	}

	if (isOpenSeparator(curChar) && curState == INITIAL_STATE)
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
	else if ((isClosedSeparator(curChar) && curState == INSIDE_SEPARATOR) ||
		 (curChar == '%'&& curState == PERCENT_SEPARATOR))
	{
		// found the closing part of a separator character
	
		// check if we have any past open separators to close
		if (!STATES_STACK.empty())
		{
			// pop the past FSM (Finite State Machine) state to the 
			//  current state 
			STATES_STACK.pop();
			return false;
		}	

		curState = END_SEPARATOR;
		return true;
	}
	else if (isOpenSeparator(curChar) && curState == INSIDE_SEPARATOR)
	{
		// need ot save the curState
		STATES_STACK.push(curState);		

		//   set curState to INITIAL
		curState = INITIAL_STATE;

		return false;
	}
	else if (isClosedSeparator(curChar) && curState == INITIAL_STATE)
	{
		// since we found a closed separators on INITIAL_STATE we
		//  may have had some past open separators
		if (!STATES_STACK.empty())
		{
			// pop the past FSM (Finite State Machine) state to the
			// current state
			curState = STATE_STACK.pop();
			return true;
		}
		else
		{
			// we have unmatched separators
			ERROR error = handleError(INSIDE_SEPARATOR);
			// ***NEED TO PRINT ERROR???
			return false;
		}
	}
	else
	{
		// separator was found in an invalid state
		ERROR error = handleError(curState);
		// ***NEED TO PRINT ERROR???
		return false;
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
bool isAcceptingState(const STATES curState)
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
//		a legal closinging separator (ex. ')', '}') in the RAT18S 
//		language.
// 
// Input: curChar - the current char we are testing to determine if its a 
//		    legal separator character.
//
// Output: true - curChar is a legal separator character for RAT18S.
//	   false - curChar is an illegal separator character for RAT18S.
//
//==============================================================================
bool isClosedSeparator(const char curChar)
{
	for (int i = 0; i < CLOSED_SEPARATOR_AMOUNT; i++)
	{
		if (curChar == CLOSED_SEPARATORS[i])	return true;
	}

	return false;
}


//==============================================================================
// Description: This function will determine if the char passed by argument is
//		a legal opening separator (ex. '(', '{') in the RAT18S language.
// 
// Input: curChar - the current char we are testing to determine if its a 
//		    legal separator character.
//
// Output: true - curChar is a legal separator character for RAT18S.
//	   false - curChar is an illegal separator character for RAT18S.
//
//==============================================================================
bool isOpenSeparator(const char curChar)
{
	for (int i = 0; i < OPEN_SEPARATOR_AMOUNT; i++)
	{
		if (curChar == OPEN_SEPARATORS[i])	return true;
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
bool isWhiteSpace(const char curChar)
{
	for (int i = 0; i < WHITESPACE_AMOUNT; i++)
	{
		if (curChar == WHITESPACES[i])	return true;
	}

	return false;
}




