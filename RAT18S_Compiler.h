// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages

#ifndef RAT18S_Compiler_HEADER
#define RAT18S_Compiler_HEADER

#include <string>


// GLOBAL CONSTANT VARIABLES
const int KEYWORDS_AMOUNT   = 12;
const int OPERATOR_AMOUNT   = 8;
const int SEPARATOR_AMOUNT  = 8;
const int WHITESPACE_AMOUNT = 4;


// stores all keywords available in RAT18S
// THIS MUST BE SORTED!!!
const std::string KEYWORDS[KEYWORDS_AMOUNT] = {"BOOLEAN", "ELSE", "ENDIF", "FALSE",
    "GET", "IF", "INT", "PUT", "REAL",
    "RETURN", "TRUE", "WHILE"};

// stores all separators available in RAT18S
const char SEPARATORS[SEPARATOR_AMOUNT] = {';' , ',' , ':' , '(' , ')' , '{' , '}' , '%' };

// stores all legal whitespace characters available in RAT18S
const char WHITESPACES[WHITESPACE_AMOUNT] = {' ', '\t', '\n', '\0'};

// stores possible BEGINNINGS (ONLY) of operators to check if a char is an operator
const char OPERATORS[OPERATOR_AMOUNT] = {'=', '+', '-', '*', '/', '<', '>', '^'};

// stores every other state of our FSM (Finite State Machine)
enum STATE
{
    INITIAL_STATE,        // first state of the FSM
    
    //STATE FOR IDENTIFIERS
    INSIDE_IDENTIFIER,    // only when it sees a digit after letter
    COULD_END_IDENTIFIER,    // only last letter, or letter in the middle
    DOLLAR_IDENTIFIER,    // only last character can be $ if its found
    END_IDENTIFIER,        // AN ACCEPTING STATE IDENTIFIERS
    
    //STATE FOR NUMBERS
    INSIDE_NUMBER,        // a digit is found and current state is initial
    END_INT,        // AN ACCEPTING STATE FOR INTEGERS
    INSIDE_REAL,        // when a '.' is found after a digit is
    COULD_END_REAL,        // when a digit is found after a '.'
    END_REAL,        // AN ACCEPTING STATE FOR REALS
    
    //STATE FOR KEYWORDS
    END_KEYWORD,        // AN ACCEPTING STATE FOR KEYWORDS
    
    //STATE FOR OPERATORS
    CARROT_OPERATOR,    // found an initial ^
    EQUAL_OPERATOR,        // found =
    POTENTIAL_OPERATOR,    // found +, -, /, *, >, <
    END_OPERATOR,        // AN ACCEPTING STATE FOR OPERATORS
    
    //STATE FOR SEPARATORS
    INSIDE_SEPARATOR,    // found {, (
    PERCENT_SEPARATOR,    // found %
    END_SEPARATOR,        // AN ACCEPTING STATE FOR SEPARATORS
    
    //STATE FOR COMMENTS
    INSIDE_COMMENT,        // found initial !
    END_COMMENT        // found last !
};


// stores every error flag that may occur in our FSM (Finite State Machine)
enum ERROR
{
    //ERROR FOR ERRORS HAHA
    UNKNOWN_ERROR,
    
    //ERRORS FOR IDENTIFIERS
    INVALID_IDENTIFIER,    // Ex a8, a$2
    
    //ERRORS FOR NUMBERS
    INVALID_INT,        // Ex 8a
    INVALID_REAL,        // Ex 7.7. .9
    
    //ERRORS FOR OPERATORS
    INVALID_OPERATOR,    // Ex **, ++, --
    
    //ERRORS FOR SYMBOLS
    INVALID_SYMBOL        //Ex @, #, &, "
};


// stores the different types a token can be
enum TOKEN_TYPE
{
    TYPE_ERROR,
    IDENTIFIER,
    KEYWORD,
    INT,
    REAL,
    OPERATOR,
    SEPARATOR,
    COMMENT,
    SYMBOL
};


struct TOKEN
{
    TOKEN_TYPE tokenType;    // store the type of token was found (ex. Real, Int, Separator)
    std::string lexeme;    // store the actual instance of the token
};


class RAT18S_Compiler
{
    
public:
    RAT18S_Compiler():lineNum(1),colmNum(0),fsmState(INITIAL_STATE) {};
    TOKEN Lexer(std::ifstream& source); // this function should return a list
    //  of tokens or print them inside it
    
    // LEXER FSMs AND HELPER FUNCTIONS
private:
    // LEXER FSMs
    bool CheckIdentifier(const char curChar);
    bool CheckKeyword();
    bool CheckNumber(const char curChar);
    bool CheckOperators(const char curChar);
    bool CheckSeparators(const char curChar);
    bool EndOfToken(const char curChar, const bool endFile);
    void FSM(const char curChar);
    
    // LEXER HELPER FUNCTIONS
    void AppendToLexeme(const char curChar) {token.lexeme += curChar;}
    void CapitalizeChar(char& curChar);
    void ClearLexeme() {token.lexeme.clear();}
    ERROR DetermineError();
    long long int GetColmNum() {return colmNum;}
    STATE GetCurrentState() {return fsmState;}
    std::string GetLexeme(){return token.lexeme;}
    long long int GetLineNum() {return lineNum;}
    TOKEN_TYPE GetToken() {return token.tokenType;}
    void SetColmNum(const long long int newColmNum) {colmNum = newColmNum;}
    void SetCurrentState(const STATE state) {fsmState = state;}
    void SetLineNum(const long long int newLineNum) {lineNum = newLineNum; colmNum = 0;}
    void SetToken(const TOKEN_TYPE token_type) {token.tokenType = token_type;}
    void RemoveLastCharLexeme() {token.lexeme.pop_back();}
    bool IsInAcceptingState();
    bool IsComment(const char curChar);
    bool IsOperator(const char curChar);
    bool IsSeparator(const char curChar);
    bool IsWhiteSpace(const char curChar);
    void IncrementFileCounters(const char curChar);
    void PrintError(const ERROR errorType);
    
    // LEXER PRIVATE DATA MEMBERS
    long long int lineNum;    // stores the line number for the inputfile
    long long int colmNum;    // stores the column number for the inputfile
    TOKEN token;        // stores the current token
    STATE fsmState;        // stores the current state of the FSM
};

#endif    // END OF RAT18Scompiler_HEADER

