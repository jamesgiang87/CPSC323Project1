// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages

//*** NEED TO BE ANSWERED QUESTIONS LEAD WITH ***
// ALL FUNCTIONS ARE LISTED IN ALPHABETICAL ORDER REGARDLESS OF PUBLIC/PRIVATE SCOPE
// BUT ALL FUNCTIONS ARE RELEVANT TO TASK IN COMPILER (Ex Lexical Analysis, Syntax Analysis..)

#include <cctype>
#include <fstream>
#include <iostream>		
#include "RAT18S_Compiler.h"
 
// ========================= Lexical Analysis Start ============================

//==============================================================================
// Description: This function will capitalize a character if lowercase letter.
//
// Input: curChar - the char we want to upper case if possible.
//
// Output: NONE
//
//==============================================================================
void RAT18S_Compiler::CapitalizeChar(char& curChar)
{
	// if curChar is an upper case letter or a symbol dont need to 
	//  upper case...
	if (curChar >= 'a' && curChar <= 'z')
	{
		// did this to reduce library code needed. 32 is difference 
		//  between uppercase ascii and lower case ascii
		curChar = curChar - 32;
	}
}


//==============================================================================
// Description: This function will check and change the FSM (Finite State
//		Machine) state if the current character is apart of a legal
//		identifier else it will throw an ERROR type.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool RAT18S_Compiler::CheckIdentifier(const char curChar)
{
	STATE curState = GetCurrentState();
	
	if (std::isalpha(curChar) && (curState == INITIAL_STATE 
	 || curState == INSIDE_IDENTIFIER || curState == COULD_END_IDENTIFIER))
	{
		// identifiers must lead with a letter or end with a
		// letter with the exception of ending with a $
		SetCurrentState(COULD_END_IDENTIFIER);
		return true;
		
	}
	else if(curChar == '$' && (curState == INSIDE_IDENTIFIER || 
				   curState == COULD_END_IDENTIFIER))
	{  
		// a $ cannot appear at the front of an identifier
		// only inside or at the end of an identifier
		SetCurrentState(DOLLAR_IDENTIFIER);
		return true;
	}
	else if(std::isdigit(curChar) && (curState == INSIDE_IDENTIFIER 
				    || curState == COULD_END_IDENTIFIER))
	{
		// an identifier cannot end in a digit so it must still
		// be considered inside and identifier
		SetCurrentState(INSIDE_IDENTIFIER);
		return true;

	}
	
	// we do not throw an error because if a digit is found
	// if may be apart of an integer or real number

	return false;
}


//==============================================================================
// Description: This function will determine if a given token is a keyword or
//		not. If the token is a keyword the current State of the FSM
//		(Finite State Machine) will be updated accordingly.
//
// Input: NONE
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//	   false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool RAT18S_Compiler::CheckKeyword()
{
	// stores the current lexeme
	std::string lexeme = GetLexeme();

	// highest index of the list we are searching
	int maxIndex = KEYWORDS_AMOUNT;	
	
	// lowest index of the list we are searching
	int minIndex = 0;	
	
	// stores where we are current looking in the keyword list
	int curIndex = minIndex + (maxIndex - minIndex)/2;	
	
	do
	{
		if (lexeme == KEYWORDS[curIndex])
		{
			SetCurrentState(END_KEYWORD);
			SetToken(KEYWORD);
			return true;
		}
		else if (lexeme < KEYWORDS[curIndex])
		{
			maxIndex = curIndex - 1;
			curIndex = minIndex + (maxIndex - minIndex)/2; 
		}
		else //curToken > KEYWORDS[curIndex]
		{
			minIndex = curIndex + 1;
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
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//	   false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool RAT18S_Compiler::CheckNumber(const char curChar)
{
	bool isDigit = std::isdigit(curChar);
	STATE curState = GetCurrentState();

	if (isDigit && curState == INITIAL_STATE)
	{
		// found a number so we are inside a number
		SetCurrentState(INSIDE_NUMBER);
		return true;
	}
	else if (isDigit && curState == INSIDE_NUMBER)
	{
		return true;
	}
	else if (curChar == '.')
	{
		if (curState == INSIDE_NUMBER)
		{
			SetCurrentState(INSIDE_REAL);
			return true;	
		}
		else
		{
			throw DetermineError();
		}
	}
	else if (isDigit && (curState == INSIDE_REAL || curState == COULD_END_REAL))
	{
		// a digit followed after the '.' and possibly more digits found
		SetCurrentState(COULD_END_REAL);
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
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//	   false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool RAT18S_Compiler::CheckOperators(const char curChar)
{
	STATE curState = GetCurrentState();

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
				SetCurrentState(END_OPERATOR);
				SetToken(OPERATOR);
				return true;
			}
			else
			{	// invalid symbol found in wrong state
				throw DetermineError();
			}

			break;

		case '^':
			if (curState == INITIAL_STATE)
			{
				// found the first part of ^= operator
				// need special enum because we cant have ^>
				// or ^space as an operator
				SetCurrentState(CARROT_OPERATOR);
				return true;
			}
			else
			{
				// invalid symbol found in wrong state
				throw DetermineError();
			}

			break;

		case '>':
		case '<':	
			if (curState == INITIAL_STATE)
			{
				// found either <, or > operator
				SetCurrentState(POTENTIAL_OPERATOR);
				return true;
			}
			else if ( curState == EQUAL_OPERATOR)
			{	
				// found portion of =>, or =< operator
				SetCurrentState(POTENTIAL_OPERATOR);
				return true;
			}
			else
			{
				// invalid symbol found in wrong state
				throw DetermineError();
			}

			break;

		case '=':	
			if (curState == INITIAL_STATE)
			{
				// found first portion of ==, =>, or =<
				SetCurrentState(EQUAL_OPERATOR);
				return true;
			}
			else if (curState == CARROT_OPERATOR)
			{
				// found the ^= operator
				SetCurrentState(END_OPERATOR);
				SetToken(OPERATOR);
				return true;
			}
			else if (curState == EQUAL_OPERATOR)
			{
				// found the == operator
				SetCurrentState(END_OPERATOR);
				SetToken(OPERATOR);
				return true;
			}
			else
			{
				// invalid symbol found in wrong state
				throw DetermineError();
			}

			break;

		default:
			break;
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
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//	   false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool RAT18S_Compiler::CheckSeparators(const char curChar)
{
	STATE curState = GetCurrentState();

	if (IsSeparator(curChar) && curState == INITIAL_STATE)
	{
		// found the first part of a separator character
		if (curChar == '%')
		{
			//special case as %% is a separator not {% or (% etc.
			SetCurrentState(PERCENT_SEPARATOR);
			return true;
		}
		else
		{
			// dont care for matching () {} just care that we
			// notice its a separator
			SetCurrentState(END_SEPARATOR);
			SetToken(SEPARATOR);
			return true;
		}
	}
	else if (curChar == '%'&& curState == PERCENT_SEPARATOR)
	{
		// found the closing part of a separator character
		SetCurrentState(END_SEPARATOR);
		SetToken(SEPARATOR);
		return true;
	}
	else
	{
		// separator was found in an invalid state
		throw DetermineError();
	}

	return false;
}


//==============================================================================
// Description: This function will determine an error given the current state
//		of the FSM (Finite State Machine). It will then return the
//		error as an enum ERROR type.
//
// Input: NONE
//
// Output: ERROR - returns an enum error value to the corresponding error that
//		   occured.
//
//==============================================================================
ERROR RAT18S_Compiler::DetermineError()
{
	switch(GetCurrentState())
	{
		// ERROR CASES FOR IDENTIFIERS
		case INSIDE_IDENTIFIER:
		case COULD_END_IDENTIFIER:
		case DOLLAR_IDENTIFIER:
			return INVALID_IDENTIFIER;
		
		// ERROR CASES FOR NUMBERS
		case INSIDE_NUMBER:
			return INVALID_INT;

		case INSIDE_REAL:
		case COULD_END_REAL:
			return INVALID_REAL;

		// ERROR CASES FOR OPERATORS
		case POTENTIAL_OPERATOR:
		case EQUAL_OPERATOR:
			return INVALID_OPERATOR;

		// ERROR CASE FOR SYMBOLS
		default:
			return INVALID_SYMBOL;
	}

	// ERROR CASES FOR THOSE YOU CANT THINK OF
	return UNKNOWN_ERROR;
}


//==============================================================================
// Description: This function will check the given character recieved from the
//		source file and compare it with the current FSM (Finite State
//		Machine) and change the state if it determines the end of a 
//		token was found. If the FSM (Finite State Machine) is will throw 
//		an ERROR type.
//
// Input: curChar - the current character taken from the source file for 
//		    evaluation.
//
// Output: true - End of a token has been found. 
//	   false - End of a token has not been found.
//
//==============================================================================
bool RAT18S_Compiler::EndOfToken(const char curChar)
{
	if (IsWhiteSpace(curChar) || IsSeparator(curChar) || IsOperator(curChar)
	    || IsComment(curChar) || IsStringLiteral(curChar))
	{
		switch(GetCurrentState())
		{
			// cases for IDENTIFIERS
			case COULD_END_IDENTIFIER:
			case DOLLAR_IDENTIFIER:
				// we have just found out that it is an
				//  identifier so label as accepted
				SetCurrentState(END_IDENTIFIER);
				SetToken(IDENTIFIER);
				return true;
				break;

			// cases for NUMBERS
			case INSIDE_NUMBER:
				// we have just found out that its is not a
				//  number anymore so we label it as accepted
				SetCurrentState(END_INT);
				SetToken(INT);
				return true;
				break;

			case COULD_END_REAL:
				// we just found out that it is a real number
				//  so we label it as an accepting state
				SetCurrentState(END_REAL);
				SetToken(REAL);
				return true;
				break;
		
			// cases for OPERATORS
				// not all operators need to be here
			case POTENTIAL_OPERATOR:
				// we have just found a > or < and IT MUST BE A SPACE
				//  or we found =< or >=
				if (!IsWhiteSpace(curChar))
				{
					SetCurrentState(END_OPERATOR);
					SetToken(OPERATOR);
					return true;
				}
				else
				{
					throw DetermineError();
				}
				
				break;

			case EQUAL_OPERATOR:
				// we have just found =
				// if it is =' ' or == then legal operator
				if (IsWhiteSpace(curChar) || curChar == '=')
				{
					SetCurrentState(END_OPERATOR);
					SetToken(OPERATOR);
					return true;
				}
				else
				{
					throw DetermineError();
				}
		
			// cases for SEPARATORS
			case INITIAL_STATE:
				if (IsOperator(curChar) || IsWhiteSpace(curChar) || 
				    IsSeparator(curChar))
				{
					return false;
				}
		
			// cases for COMMENTS
			case INSIDE_COMMENT:	
			case END_COMMENT:
				SetToken(COMMENT);
				return true;
				break;
			
			// cases for STRING LITERALS
			case INSIDE_STRING_LITERAL:
			case END_STRING_LITERAL:
				SetToken(TYPE_ERROR);
				return true;
				break;

			// dont need it for separatros or comments because we
			//  know when we find the second one it is accepted
			default:
				break;
		}
	}

	return false;
}


//==============================================================================
// Description: This function will encapulate all FSM (Finite State Machine)
//		functionality. This function will return true if the state of
//		the FSM (Finite State Machine) was updated/changed or false
//		if the state was unchanged.
// 
// Input: curChar - the current char recieved from the input file
//	  
// Output: NONE
//
//==============================================================================
void RAT18S_Compiler::FSM(const char curChar)
{
	STATE curState = GetCurrentState();

	if (IsWhiteSpace(curChar) && curState == INITIAL_STATE)
	{}
	// run every machines apart of the FSM
	// SPECIAL CASE FOR IDENTIFIERS
	else if (CheckIdentifier(curChar))
	{
		// check if the state has been updated
		curState = GetCurrentState();

		if (curState == COULD_END_IDENTIFIER)
		{
			CheckKeyword();
		}
	}
	else if (CheckNumber(curChar) || CheckOperators(curChar) || 
		 CheckSeparators(curChar))
	{}
}


//==============================================================================
//==============================================================================
TOKEN RAT18S_Compiler::Lexer(std::ifstream& source)
{
    
    ClearLexeme();            // make sure there are no old lexemes
    bool tokenFound = false;    // if token is found set true
    char fileChar;            // stores the chars extracted from file
    
    // look for a token until token is found or end of source file is found
    do
    {
        // retrieve a character from the file white fileChar is not whitespace
        source.get(fileChar);
            
        // Have the FSM find initial comment/string character and then
        //  this will catch the last comment/string character
        if (GetCurrentState() == INSIDE_COMMENT ||
            GetCurrentState() == INSIDE_STRING_LITERAL)
        {
            do
            {
                source.get(fileChar);
                SetColmNum(++colmNum);
                
            }while ((!IsComment(fileChar)        &&
                     !IsStringLiteral(fileChar)) &&
                     !source.eof());
            
        }
        else if (fileChar == '\n'){    SetLineNum(++lineNum);     }
        else {SetColmNum(++colmNum);}
        
        // append the fileChar to the token
        if (!IsWhiteSpace(fileChar))
      	{
	   	// capitalize the character from the file because RAT18S is
        	//  insensative to upper case or lower case names
        	CapitalizeChar(fileChar);
		AppendToLexeme(fileChar);
        }
        
        
        if (IsComment(fileChar))
        {
            if (GetCurrentState() == INITIAL_STATE)
            {
                SetCurrentState(INSIDE_COMMENT);
            }
            else
            {
                SetCurrentState(END_COMMENT);
            }
        }
        else if (IsStringLiteral(fileChar))
        {
            if (GetCurrentState() == INITIAL_STATE)
            {
                SetCurrentState(INSIDE_STRING_LITERAL);
            }
            else
            {
                SetCurrentState(END_STRING_LITERAL);
            }
        }
        
        try
        {
            // if operator or separator is encounters we need
            //  to back up the file pointer. However we dont
            //  need to move file pointer if it is whitespace
            if (EndOfToken(fileChar))
            {
		// dont push file pointer back for comments, string literals
		//  or whitespace
                if (!IsComment(fileChar) && !IsStringLiteral(fileChar))
                {
			// if its whitespace dont need to unappend... if its
			//  an operator is it a valid operator as a whole so
			//  dont unappend part of a valid operator
                    if (!IsWhiteSpace(fileChar) && 
			GetCurrentState() != END_OPERATOR)
                    {
                        // push the file pointer back
                        source.unget();
                        SetColmNum(--colmNum);    // decrease column number
                        RemoveLastCharLexeme();    
                    }
                }
            }
            else
            {
                // run all the FSM on the currentCharacter
                FSM(fileChar);
            }
            
            // check if the current state is in an acceptingState
            (IsInAcceptingState()) ? tokenFound = true :
            			     tokenFound = false;
            
        }
        catch(const ERROR error)
        {
		// dont want to unappend part of error token.. we only need to
		//  unappend when we have to search for new token
		bool foundNewToken = false;

	   	// while we have found an error we need to find the next token
		while(!IsWhiteSpace(fileChar) && !IsOperator(fileChar) && 
			!IsSeparator(fileChar) && !IsComment(fileChar) && 
			!IsStringLiteral(fileChar) && !source.eof())
		{
			foundNewToken = true;
		 	source.get(fileChar);
		
			// append all characters associated with error
			AppendToLexeme(fileChar);
	
			// if we encounter new lines, whitespace then increment
			//  file counters accordingly
			IncrementFileCounters(fileChar);
		}

		if (foundNewToken)
		{
			// remove last character associated with 
			//  new token as not part of error
			RemoveLastCharLexeme();
	
			// we found our next token so put it back and reset the FSM state
	  		if (!IsWhiteSpace(fileChar))
	  		{
				// if not whitespace then we need to store first token
				source.unget();
	  		}
		}
 
	  	// print the error to the user
      		PrintError(error);

		SetCurrentState(INITIAL_STATE);

	 	// clear current token weve been storing (cus it was an error)
	 	ClearLexeme();
       }
        
    } while (!tokenFound && !source.eof());
    
    return token;
}

//==============================================================================
// Description: This function will determine if the current state of the FSM
//		(Finite State Machine) is in an accepting state or not. 
// 
// Input: NONE
//
// Output: true - the current state of the FSM (Finite State Machine) is in an
//		  accepting state.
//	   false - the current state of the FSM (Finite State Machine) is not
//		   in an accepting state.
//
//==============================================================================
bool RAT18S_Compiler::IsInAcceptingState()
{
	switch(fsmState)
	{
		case END_IDENTIFIER:
		case END_INT:
		case END_REAL:
		case END_KEYWORD:
		case END_OPERATOR:
		case END_SEPARATOR:
		case END_COMMENT:
		case END_STRING_LITERAL:
			SetCurrentState(INITIAL_STATE);	// reset the FSM
			return true;	// an accepting state

		// special cases where the FSM is not reset
		case INSIDE_COMMENT:
		case INSIDE_STRING_LITERAL:
			return true;
		default:
			return false;	// not an accepting state
	}	

	return false;
}


//==============================================================================
// Description: This function will determine if the char passed by argument is
//		a legal comment character in the RAT18S language.
// 
// Input: curChar - the current char we are testing to determine if its a 
//		    legal comment character.
//
// Output: true - curChar is a legal comment character for RAT18S.
//	   false - curChar is an illegal comment char for RAT18S.
//
//==============================================================================
bool RAT18S_Compiler::IsComment(const char curChar)
{
	return curChar == '!';
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
bool RAT18S_Compiler::IsOperator(const char curChar)
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
bool RAT18S_Compiler::IsSeparator(const char curChar)
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
bool RAT18S_Compiler::IsStringLiteral(const char curChar)
{

	if (curChar == '\"') 	return true;
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
bool RAT18S_Compiler::IsWhiteSpace(const char curChar)
{
	for (int i = 0; i < WHITESPACE_AMOUNT; i++)
	{
		if (curChar == WHITESPACES[i])	return true;
	}

	return false;
}


//==============================================================================
// Description: This function will incremement the column number and line 
//		number if needed.
//
// Input:	curChar - the current character pulled from the input file.
//
// Output: NONE
//
//==============================================================================
void RAT18S_Compiler::IncrementFileCounters(const char curChar)
{
	if (curChar == ' ' || curChar == '\t')
	{
		SetColmNum(GetColmNum()+1);
	}
	else if (curChar == '\n')
	{
		SetLineNum(GetLineNum()+1);
	}
}


//==============================================================================
// Description: This function will print out a given Error along with the 
//		possible token we believe is in error as well as what line 
//		number and column number this error can be found.
//
// Input:	errorType - the type of error that was caused
//
// Output: NONE
//
//==============================================================================
void RAT18S_Compiler::PrintError(const ERROR errorType)
{

	std::cout << GetLineNum() << ":" << GetColmNum() << " RAT18S error: ";

	switch(errorType)
	{
		case INVALID_IDENTIFIER:
			std::cout << " invalid identifier \"" << GetLexeme() << "\"" << std::endl;
			break;
					
		case INVALID_INT:
			std::cout << " invalid integer \"" << GetLexeme() << "\"" << std::endl;
			break;
				
		case INVALID_REAL:
			std::cout << " invalid real \"" << GetLexeme() << "\"" << std::endl;
			break;
			
		case INVALID_OPERATOR:
			std::cout << " invalid operator \"" << GetLexeme() << "\"" << std::endl;
			break;

		case INVALID_SYMBOL:
			std::cout << " invalid symbol \"" << GetLexeme() << "\"" << std::endl;
			break;
	
		default:
			 // unknown error
			std::cout << " unknown error occured." << std::endl;
			break;
	}
}



// ========================== Lexical Analysis End =============================
