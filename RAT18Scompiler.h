// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages

#ifndef RAT18Scompiler_HEADER
#define RAT18Scompiler_HEADER

#include <string>


// GLOBAL CONSTANT VARIABLES
const int KEYWORDS_AMOUNT   = 12;
const int OPERATOR_AMOUNT   = 8;
const int SEPARATOR_AMOUNT  = 6;
const int WHITESPACE_AMOUNT = 3;


// stores all keywords available in RAT18S
// THIS MUST BE SORTED!!!
const std::string KEYWORDS[KEYWORDS_AMOUNT] = {"BOOLEAN", "ELSE", "ENDIF", "FALSE", 
					       "GET", "IF", "INT", "PUT", "REAL", 
					       "RETURN", "TRUE", "WHILE"}; 

// stores all separators available in RAT18S
const char SEPARATORS[SEPARATOR_AMOUNT] = {';' , '(' , ')' , '{' , '}' , '%' };

// stores all legal whitespace characters available in RAT18S
const char WHITESPACES[WHITESPACE_AMOUNT] = {' ', '\t', '\n'};

// stores possible BEGINNINGS (ONLY) of operators to check if a char is an operator 
const char OPERATORS[OPERATOR_AMOUNT] = {'=', '+', '-', '*', '/', '<', '>', '^'};

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
	SYMBOL
	//, TYPE_ERROR	// ***NOT SURE IF NEEDED
};


struct TOKEN
{
	TOKEN_TYPE tokenType;	// store the type of token was found (ex. Real, Int, Separator)
	std::string lexeme;	// store the actual instance of the token
};


class RAT18Scompiler
{

public:
	RAT18Scompiler() {};
	TOKEN Lexer(std::ifstream& source); // this function should return a list 
				           //  of tokens or print them inside it

// LEXER FSMs AND HELPER FUNCTIONS
private:
// LEXER FSMs
	bool CheckBackup(const char curChar, STATES& curState, std::ifstream source);
	bool CheckComments(const char curChar, STATES& curState);
	bool CheckIdentifier(const char curChar, STATES& curState);
	bool CheckKeyword(const std::string curToken, STATES& curState);
	bool CheckNumber(const char curChar, STATES& curState);
	bool CheckOperators(const char curChar, STATES& curState);
	bool CheckSeparators(const char curChar, STATES& curState);
	bool FSM(const char curChar, STATES& curState, const std::string curToken);

// LEXER HELPER FUNCTIONS
	ERROR DetermineError(const STATES curState);
	bool IsAcceptingState(const STATES curState);
	bool IsOperator(const char curChar);
	bool IsSeparator(const char curChar);
	bool IsStringLiteral(const char curChar);
	bool IsWhiteSpace(const char curChar);
	void PrintError(const int lineNum, const int colmNum, 
			const std::string token, const ERROR errorType);
};

#endif	// END OF RAT18Scompiler_HEADER
