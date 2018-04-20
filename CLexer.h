// Date: 3/14/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
// File: CLexer.h

#ifndef CLexer_HEADER
#define CLexer_HEADER

#include <string>
#include <fstream>

// GLOBAL CONSTANT VARIABLES
const int KEYWORDS_AMOUNT   = 13;
const int OPERATOR_AMOUNT   = 8;
const int SEPARATOR_AMOUNT  = 10;
const int WHITESPACE_AMOUNT = 4;

// stores all keywords available in RAT18S
// THIS MUST BE SORTED!!!
const std::string KEYWORDS[KEYWORDS_AMOUNT] = {"BOOLEAN", "ELSE", "ENDIF", "FALSE",
    "FUNCTION", "GET", "IF", "INT", "PUT", "REAL", "RETURN", "TRUE", "WHILE"};

// stores all separators available in RAT18S
const char SEPARATORS[SEPARATOR_AMOUNT] = {';' , ',' , ':' , '(' , ')' , '{' , '}' , '[' , ']' , '%' };

// stores all legal whitespace characters available in RAT18S
const char WHITESPACES[WHITESPACE_AMOUNT] = {' ', '\t', '\n', '\0'};

// stores possible BEGINNINGS (ONLY) of operators to check if a char is an operator
const char OPERATORS[OPERATOR_AMOUNT] = {'=', '+', '-', '*', '/', '<', '>', '^'};

// stores every other state of our FSM (Finite State Machine)
enum State
{
    INITIAL_STATE,        // first state of the FSM
    
    //STATE FOR IDENTIFIERS
    INSIDE_IDENTIFIER,    // only when it sees a digit after letter
    COULD_END_IDENTIFIER, // only last letter, or letter in the middle
    DOLLAR_IDENTIFIER,    // only last character can be $ if its found
    END_IDENTIFIER,       // AN ACCEPTING STATE IDENTIFIERS
    
    //STATE FOR INTEGER
    INSIDE_INTEGER,     // a digit is found and current state is initial
    END_INTEGER,           // AN ACCEPTING STATE FOR INTEGERS
    INSIDE_REAL,        // when a '.' is found after a digit is
    COULD_END_REAL,     // when a digit is found after a '.'
    END_REAL,            // AN ACCEPTING STATE FOR REALS
    
    //STATE FOR KEYWORDS
    END_KEYWORD,        // AN ACCEPTING STATE FOR KEYWORDS
    
    //STATE FOR OPERATORS
    CARROT_OPERATOR,    // found an initial ^
    EQUAL_OPERATOR,     // found =
    POTENTIAL_OPERATOR, // found +, -, /, *, >, <
    END_OPERATOR,       // AN ACCEPTING STATE FOR OPERATORS
    
    //STATE FOR SEPARATORS
    PERCENT_SEPARATOR,  // found %
    END_SEPARATOR       // AN ACCEPTING STATE FOR SEPARATORS
    
};


// stores every error flag that may occur in our FSM (Finite State Machine)
enum Error_CLexer
{
    //ERROR FOR ERRORS HAHA
    CLEXER_UNKNOWN_ERROR,
    
    //ERRORS FOR IDENTIFIERS
    INVALID_IDENTIFIER,    // Ex a8, a$2
    
    //ERRORS FOR NUMBERS
    INVALID_INTEGER,     // Ex 8a
    INVALID_REAL,        // Ex 7.7. .9
    
    //ERRORS FOR OPERATORS
    INVALID_OPERATOR,    // Ex **, ++, --
    
    //ERRORS FOR SYMBOLS
    INVALID_SYMBOL        //Ex @, #, &, "
};


// stores the different types a token can be
enum Token_Type
{
    TYPE_ERROR,
    IDENTIFIER,
    KEYWORD,
    INT_VALUE,
    REAL_VALUE,
    OPERATOR,
    SEPARATOR,
    SYMBOL
};


class CToken
{
public:
    CToken(): m_tokenType(TYPE_ERROR), m_lexeme("") {};
    
    // HELPER FUNCTIONS
    void AppendToLexeme(const char curChar) {m_lexeme += curChar;}
    inline void ClearLexeme() {m_lexeme.clear();}
    Token_Type GetTokenType() const {return m_tokenType;}
    std::string GetLexeme() const {return m_lexeme;}
    long long GetLexemeLength() const {return m_lexeme.length();}
    bool IsLexemeEmpty() {return m_lexeme.empty();}
    CToken& operator=(const CToken& lhs) {m_tokenType = lhs.GetTokenType();
                                      m_lexeme = lhs.GetLexeme(); return *this; }
    void PrintTokenType() const;
    void SetToken(const CToken token) {m_tokenType = token.GetTokenType();
                                       m_lexeme = token.GetLexeme();}
    void SetTokenType(const Token_Type tokenType) {m_tokenType = tokenType;}
    inline void RemoveLastCharLexeme() {m_lexeme.pop_back();}
    
private:
    // store the type of token was found (ex. Real, Int, Separator)
    Token_Type m_tokenType;
    
    // store the actual instance of the token
    std::string m_lexeme;
};



class CLexer
{
    
public:
    CLexer(): m_fsmState(INITIAL_STATE), m_lineNum(1), m_colmNum(0) {};
    CToken Lexer();
    CLexer(const CToken& token) {m_token = token;}
    
    CLexer& operator=(const CLexer& lhs) {
        m_token = lhs.GetToken();
        m_fsmState = lhs.GetCurrentState();
       // m_inputFile = lhs.GetFile();
        m_lineNum = lhs.GetLineNum();
        m_colmNum = lhs.GetColmNum();
        return *this;
    }
    
    // LEXER FSMs
    bool CheckIdentifier(const char curChar);
    bool CheckKeyword(const char curChar);
    bool CheckNumber(const char curChar);
    bool CheckOperators(const char curChar);
    bool CheckSeparators(const char curChar);
    bool EndOfToken(const char curChar, const bool endFile);
    void FSM(const char curChar);
    
    // LEXER HELPER FUNCTIONS
    void AppendToLexeme(const char curChar) {m_token.AppendToLexeme(curChar);}
    void CapitalizeChar(char& curChar);
    inline void ClearLexeme() {m_token.ClearLexeme();}
    
    State GetCurrentState() const {return m_fsmState;}
    std::string GetLexeme() const {return m_token.GetLexeme();}
    long long GetLexemeLength() const {return m_token.GetLexemeLength();}
    CToken GetToken() const {return m_token;}
    Token_Type GetTokenType() const {return m_token.GetTokenType();}
    void PrintTokenType() const {m_token.PrintTokenType();}
    void SetCurrentState(const State state) {m_fsmState = state;}
    void SetTokenType(const Token_Type token_type) {m_token.SetTokenType(token_type);}
    void SetToken(const CToken& token) {m_token = token;}
    inline void RemoveLastCharLexeme() {m_token.RemoveLastCharLexeme();}
    bool IsInAcceptingState();
    bool IsComment(const char curChar);
    bool IsOfTypeOperator();
    bool IsOperator(const char curChar);
    bool IsSeparator(const char curChar);
    bool IsWhiteSpace(const char curChar);
    
    // ERROR FUNCTIONS
    Error_CLexer DetermineError();
    long long CalcErrorOffset() const;
    void PrintError(const Error_CLexer errorType);
    
    // FILE FUNCTIONS
    inline void CloseFile() {m_inputFile.close();}
    inline bool Eof() {return m_inputFile.eof();}
    inline bool FileFail() {return m_inputFile.fail();}
    inline bool FileGood() {return m_inputFile.good();}
    inline void OpenFile(char* fileName) {m_inputFile.open(fileName);}
    inline void OpenFile(std::string fileName) {m_inputFile.open(fileName);}
    
    // LINE/COLUMN COUNTING FUNCTIONS
    long long int GetColmNum() const {return m_colmNum;}
    long long int GetLineNum() const {return m_lineNum;}
    void SetColmNum(const long long int newColmNum)
    {m_colmNum = newColmNum; if (m_colmNum <= 0){m_colmNum = 1;}}
    void SetLineNum(const long long int newLineNum)
    {m_lineNum = newLineNum;
        if (m_lineNum <= 0) {m_lineNum = 1;}
        m_colmNum = 0;}
    void IncrementFileCounters(const char curChar);
    
private:
    CToken m_token;       // stores the current token
    State m_fsmState;     // stores the current state of the FSM
    
    // FILE DATA MEMBERS
    std::ifstream m_inputFile;
    
    long long int m_lineNum;    // stores the line number for the inputfile
    long long int m_colmNum;    // stores the column number for the inputfile
    
};

#endif    // END OF CLexer_HEADER



