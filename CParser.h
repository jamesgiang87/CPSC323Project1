//Date: 3/24/2018
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
// File: Cparser.h

#ifndef CParser_HEADER
#define CParser_HEADER

#include "CLexer.h"
#include "CTables.h"

// controls whether or not the rules for each production is printed to screen
const bool PRINT_RULE_STATEMENTS = false;

enum Error_CParser
{
    CPARSER_UNKNOWN_ERROR,
    EXPECTED_CONDITION,
    EXPECTED_EXPRESSION,
    EXPECTED_IDENTIFIER,
    EXPECTED_KEYWORD,
    EXPECTED_NOTHING,
    EXPECTED_OPERATOR,
    EXPECTED_RELOP,
    EXPECTED_SEPARATOR,
    EXPECTED_STATEMENT,
    EXPECTED_QUALIFIER
};

// a code used to help user know what type of token was expected (missing or
//  written incorrectly)
enum Exp_Token_Type
{
    UNKNOWN,        // wont attempt to help user as may be multiple options
    TOKEN_FOUND,    // will print whatever symbol was found
    PERCENT,        // %% rule 1
    FUNCTION,       // function rule 4
    LEFT_BRACKET,   // [ rule 4
    RIGHT_BRACKET,  // ] rule 4
    COMMA,          // , rule 6, 13
    COLON,          // : rule 7
    SEMI_COLON,     // ; rule 11, 17, 19, 20, 21
    LEFT_BRACE,     // { rule 16
    RIGHT_BRACE,    // } rule 16
    EQUAL,          // = rule 17
    IF,             // if rule 18
    LEFT_PAREN,     // ( rule 18, 20, 21, 22, 28
    RIGHT_PAREN,    // ) rule 18, 20, 21, 22, 28
    ENDIF,          // rule 18
    RETURN,         // rule 19
    PUT,            // rule 20
    GET,            // rule 21
    WHILE           // rule 22
};



class CParser
{
    public:
        CParser(): m_needToken(true), m_ErrorThrown(false),
                   m_Error(CPARSER_UNKNOWN_ERROR),
                   m_Error_Exp_Token(UNKNOWN) {}
    
        // FOR SYNTAX
        bool Rat18S(CLexer& token);                      // rule 1
        bool OptFunctionDefinitions(CLexer& token);      // rule 2
        bool FunctionDefinitions(CLexer& token);         // rule 3
        bool FunctionDefinitionsPrime(CLexer& token);    // rule 3.1
        bool Function(CLexer& token);                    // rule 4
        bool OptParameterList(CLexer& token);            // rule 5
        bool ParameterList(CLexer& token);               // rule 6
        bool ParameterListPrime(CLexer& token);          // rule 6.1
        bool Parameter(CLexer& token);                   // rule 7
        bool Qualifier(CLexer& token);                   // rule 8
        bool Body(CLexer& token);                        // rule 9
        bool OptDeclarationList(CLexer& token);          // rule 10
        bool DeclarationList(CLexer& token);             // rule 11
        bool DeclarationListPrime(CLexer& token);        // rule 11.1
        bool Declaration(CLexer& token);                 // rule 12
        bool IDs(CLexer& token);                         // rule 13
        bool IDsPrime(CLexer& token);                    // rule 13.1
        bool StatementList(CLexer& token);               // rule 14
        bool StatementListPrime(CLexer& token);          // rule 14.1
        bool Statement(CLexer& token);                   // rule 15
        bool Compound(CLexer& token);                    // rule 16
        bool Assign(CLexer& token);                      // rule 17
        bool If(CLexer& token);                          // rule 18
        bool IfPrime(CLexer& token);                     // rule 18.1
        bool Return(CLexer& token);                      // rule 19
        bool ReturnPrime(CLexer& token);                 // rule 19.1
        bool Print(CLexer& token);                       // rule 20
        bool Scan(CLexer& token);                        // rule 21
        bool While(CLexer& token);                       // rule 22
        bool Condition(CLexer& token);                   // rule 23
        bool Relop(CLexer& token);                       // rule 24
        bool Expression(CLexer& token);                  // rule 25
        bool ExpressionPrime(CLexer& token);             // rule 25.1
        bool Term(CLexer& token);                        // rule 26
        bool TermPrime(CLexer& token);                   // rule 26.1
        bool Factor(CLexer& token);                      // rule 27
        bool Primary(CLexer& token);                     // rule 28
        bool IdentifierPrime(CLexer& token);             // rule 28.1
        bool Empty(CLexer& token);                       // rule 29
    
        // SYMBOL TABLE FUNCTIONS
        void PrintSymbolTable() const {m_symbolTable.List();}
        void PrintInstrTable() const {m_instrTable.PrintTable();}
    
    private:
        bool TokenNeeded() {return m_needToken;}
        void SetTokenNeeded(bool used) {m_needToken = used;}
        void GetToken(CLexer& token);
        void SetError(const Error_CParser error, const Exp_Token_Type expToken);
        void SetErrorType(const Error_CParser error) {m_Error = error;}
        void SetErrorThrown(const bool thrown) {m_ErrorThrown = thrown;}
        bool ErrorThrown() {return m_ErrorThrown;}
        Error_CParser GetErrorType() {return m_Error;}
        void SetErrorExpTokenType(const Exp_Token_Type expToken)
                         {m_Error_Exp_Token = expToken;}
        Exp_Token_Type FindErrorExpTokenType(const CLexer& token);
        Exp_Token_Type GetErrorExpTokenType() {return m_Error_Exp_Token;}
        void PrintExpTokenType(const CLexer& token);
        void PrintError(const Error_CParser error, CLexer& token);
        void PrintRule(const std::string output, const CLexer& token);
        void PrintToken(const CLexer& token);
    
        // SYMBOL TABLE PRIVATE MEMBER FUNCTIONS
        void SetVariableType(const VariableTypes varType)
                                    {m_symbolTable.SetRecVarTypeUsed(varType);}
        void InsertVariable(const CToken& var) {m_symbolTable.Insert(var);}
        void SetDeclaringVar(const bool state)
                                        {m_symbolTable.SetDeclaringVar(state);}
        void PrintError(const Error_SymbolTable error, const CLexer& token)const
                                    {m_symbolTable.PrintError(error, token);}
    
        bool m_needToken;
        bool m_ErrorThrown;
        Error_CParser m_Error;
        Exp_Token_Type m_Error_Exp_Token;
    
        // SYMBOL TABLE
        CSymbolTable m_symbolTable;
    
        // INSTRUCTION TABLE
        CInstructionTable m_instrTable;
};


#endif //END OF CParser_HEADER


