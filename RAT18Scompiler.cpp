// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages

//*** NEED TO BE ANSWERED QUESTIONS LEAD WITH ***
// ALL FUNCTIONS ARE LISTED IN ALPHABETICAL ORDER REGARDLESS OF PUBLIC/PRIVATE SCOPE


#include <cctype>
#include <fstream>
#include <iostream>		
#include "RAT18Scompiler.h"
 

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
//	  source - a reference to the input file stream.
//
// Output: true - the file pointer was backed up. 
//	   false - the file pointer was not backed up.
//
//==============================================================================
// ***NOT SURE IF THIS ACTUALLY NEEDS TO BE BOOL
bool RAT18Scompiler::CheckBackup(const char curChar, STATES& curState, 
				 std::ifstream source)
{// ***POTENTIALLY UNFINISHED

	bool pushBack = false;		// determines if file pointer needs
					//  to be pushed back

	if (IsWhiteSpace(curChar) || IsSeparator(curChar) || IsOperator(curChar))
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
				if (!IsOperator(curChar))
				{
					curState = END_OPERATOR;
					pushBack = true;
				}
				else
				{
					throw DetermineError(curState);
				}
				
				break;
				
			// dont need it for separatros or comments because we
			//  know when we find the second one it is accepted
			default:
				// we need to throw error
				throw DetermineError(curState);
		}

		if (pushBack)
		{
			// set the file pointer back one character
			source.unget();
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
bool RAT18Scompiler::CheckComments(const char curChar, STATES& curState)
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
		throw DetermineError(curState);
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
bool RAT18Scompiler::CheckIdentifier(const char curChar, STATES& curState)
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
		throw DetermineError(curState);
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
bool RAT18Scompiler::CheckKeyword(const std::string curToken, STATES& curState)
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
bool RAT18Scompiler::CheckNumber(const char curChar, STATES& curState)
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
			throw DetermineError(curState);
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
bool RAT18Scompiler::CheckOperators(const char curChar, STATES& curState)
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
				throw DetermineError(curState);
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
				throw DetermineError(curState);
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
				throw DetermineError(curState);
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
				throw DetermineError(curState);
			}

			break;

		default:
			throw DetermineError(curState);
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
bool RAT18Scompiler::CheckSeparators(const char curChar, STATES& curState)
{
	if (IsSeparator(curChar) && curState == INITIAL_STATE)
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
	else if ((IsSeparator(curChar) && curState == INSIDE_SEPARATOR) ||
		 (curChar == '%'&& curState == PERCENT_SEPARATOR))
	{
		// found the closing part of a separator character
		curState = END_SEPARATOR;
		return true;
	}
	else
	{
		// separator was found in an invalid state
		throw DetermineError(curState);
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
ERROR RAT18Scompiler::DetermineError(const STATES curState)
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


		// ERROR CASE FOR SYMBOLS
		default:
			return INVALID_SYMBOL;
	}

	// ERROR CASES FOR THOSE YOU CANT THINK OF
	return UNKNOWN_ERROR;
}


//==============================================================================
// Description: This function will encapulate all FSM (Finite State Machine)
//		functionality. This function will return true if the state of
//		the FSM (Finite State Machine) was updated/changed or false
//		if the state was unchanged.
// 
// Input: curChar - the current char recieved from the input file
//
//	  curState - the current state of the FSM (Finite State Machine).
//
//	  curToken - the current literal token we have "found" so far while 
//		     evaluating the input file
//	  
// Output: true - the state of the FSM (Finite State Machine) was changed.
//
//	   false - the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================

bool RAT18SCompiler::FSM(const char curChar, STATES& curState, 
			 const std::string curToken)
{

	// run every machines apart of the FSM
	// SPECIAL CASE FOR IDENTIFIERS
	if (CheckIdentifier(curChar, curState))
	{
		if (curState == END_IDENTIFIER)
		{
			CheckKeyword(curToken, curState);
			// state of FSM has been changed
			return true;
		}
	}
	else if (CheckComments(curChar, curState)   ||
	    	 CheckNumber(curChar, curState)	    ||
	    	 CheckOperators(curChar, curState)  ||
	    	 CheckSeparators(curChar, curState))
	{
		// state of FSM has beend changed
		return true;
	}
	
	// FSM state unchanged
	return false;
}


//==============================================================================
//==============================================================================
TOKEN RAT18Scompiler::Lexer(std::ifstream& source)
{
	//getchar();
	// if input char terminates a token AND it is an accepting state then
	//	Isolate the token/lexem
	//	Decrement the CP if necessary
	// else lookup FSM(current state, input char);
	//until (token found) or (no more input)
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
bool RAT18Scompiler::IsAcceptingState(const STATES curState)
{
	switch(curState)
	{
		case END_IDENTIFIER:
		case END_INT:
		case END_REAL:
		case END_KEYWORD:
		case END_OPERATOR:
		case END_SEPARATOR:
			return true;	// an accepting state
		default:
			return false;	// not an accepting state
	}	

	return false;
}


//==============================================================================
// Description: This function will determine if the char passed by argument is
//		a legal beginning of an operator only in the RAT18S language.
// 
// Input: curChar - the current char we are testing to determine if its a 
//		    legal beginning operator character.
//
// Output: true - curChar is a legal beginning operator character for RAT18S.
//	   false - curChar is an illegal beginning operator char for RAT18S.
//
//==============================================================================
bool RAT18Scompiler::IsOperator(const char curChar)
{
	for (int i = 0; i < OPERATOR_AMOUNT; i++)
	{
		if (curChar == OPERATORS[i])	return true;
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
bool RAT18Scompiler::IsSeparator(const char curChar)
{
	for (int i = 0; i < SEPARATOR_AMOUNT; i++)
	{
		if (curChar == SEPARATORS[i])	return true;
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
bool RAT18Scompiler::IsStringLiteral(const char curChar)
{

	if (curChar == "\"") 	return true;
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
bool RAT18Scompiler::IsWhiteSpace(const char curChar)
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
void RAT18Scompiler::PrintError(const int lineNum, const int colmNum, 
		const std::string token, const ERROR errorType)
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



