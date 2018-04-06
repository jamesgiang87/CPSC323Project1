// Date Created: 3/24/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages

#include "CParser.h"
#include "CLexer.h"
#include <iostream>
#include <stdlib.h>
#include <iomanip>

bool CParser::Rat18S(CLexer& token)
{
    PrintRule("\t<Rat18S> -> <Opt Function Definitions> %%", token);
    PrintRule(" <Opt Declaration List> <Statement List>\n", token);
 
    GetToken(token);
    SetTokenNeeded(false);
    OptFunctionDefinitions(token);
    
    GetToken(token);
    
    try
    {
        if ("%%" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            
            OptDeclarationList(token);
            if (!StatementList(token))
            {
                SetError(EXPECTED_STATEMENT, UNKNOWN);
                throw GetErrorType();
            }
            
            if ("}" == token.GetLexeme())
            {
                // if you have an extra } at the end of a compound statement
                SetError(EXPECTED_NOTHING, FindErrorExpTokenType(token));
                throw GetErrorType();
            }
            
            return true;
        }
        else
        {
            if (IDENTIFIER != token.GetTokenType())
            {
                SetError(EXPECTED_SEPARATOR, PERCENT);
            }
                throw GetErrorType();
        }
    }
    catch(const Error_CParser error)
    {
        PrintError(error, token);
    }
    
    return false;
}


bool CParser::OptFunctionDefinitions(CLexer& token)
{
    PrintRule("\t<Opt Function Definitions -> <Function Definitions> |", token);
    PrintRule(" <Empty>\n", token);
    if (FunctionDefinitions(token))
    {
        return true;
    }
    else if (Empty(token))
    {
        return true;
    }
    
    return false;
}


bool CParser::FunctionDefinitions(CLexer& token)
{   // *** NOT SURE WHAT TO RETURN AS IN BOOL
    
    try
    {
        if (Function(token) && FunctionDefinitionsPrime(token))
        {
            return true;
        }
    }
    catch(const Error_CParser error)
    {
        PrintError(error, token);
    }
    
    return false;
}


bool CParser::FunctionDefinitionsPrime(CLexer& token)
{
    PrintRule("\t<Funciton Definition Prime> ->", token);
    PrintRule(" <Function Definitions> | <Empty>\n", token);
    if (FunctionDefinitions(token))
    {
        return true;
    }
    
    //PrintRule("\t<Function Definition Prime> -> <Empty>\n", token);
    Empty(token);
    return true;
}


bool CParser::Function(CLexer& token)
{
    GetToken(token);
    if ("FUNCTION" == token.GetLexeme())
    {
        PrintRule("\t<Function Definitions> -> <Function>", token);
        PrintRule(" <Function Definition Prime>\n", token);
        SetTokenNeeded(true);
        GetToken(token);
        if (IDENTIFIER == token.GetTokenType())
        {
            SetTokenNeeded(true);
            GetToken(token);
            if ("[" == token.GetLexeme())
            {
                PrintRule("\t<Function> -> function <Identifier> [", token);
                PrintRule(" <Opt Parameter List> ] <Opt Declaration List> <Body>\n", token);
                OptParameterList(token);

                GetToken(token);
                if ("]" == token.GetLexeme())
                {
                    SetTokenNeeded(true);
                    if(OptDeclarationList(token) && Body(token))
                    {
                        return true;
                    }
                }
                else
                {
                    SetError(EXPECTED_SEPARATOR, RIGHT_BRACKET);
                    throw GetErrorType();
                }
            }
            else
            {
                SetError(EXPECTED_SEPARATOR, LEFT_BRACKET);
                throw GetErrorType();
            }
        }
        else
        {
                SetError(EXPECTED_IDENTIFIER, UNKNOWN);
                throw GetErrorType();
        }
    }
// *** NEED TO FIGURE OUT TO THROW ERROR FOR MISSING FUNCTION KEYWORD
    else
    {
        // by looking at the grammar we can say that if we had function
        //  the next thing we would see is an identifier so if function
        //  should have been here then this error will be detected if there
        //  is an identifier. if its not then no function was being called.
        if (IDENTIFIER == token.GetTokenType())
        {
            SetError(EXPECTED_KEYWORD, FUNCTION);
        }
    }
    
    SetTokenNeeded(false);
    return false;
}


bool CParser::OptParameterList(CLexer& token)
{
    PrintRule("\t<Opt Parameter List> -> <Parameter List> | <Empty>\n", token);
    
    if (ParameterList(token))
    {
        return true;
    }
    else if (Empty(token))
    {
        return true;
    }
    
    return false;
}


bool CParser::ParameterList(CLexer& token)
{ // *** NOT ENTIRELY SURE THIS IS CORRECT
    PrintRule("\t<Parameter List> -> <Parameter>", token);
    PrintRule(" <Parameter List Prime>\n", token);
    
    if (Parameter(token))
    {
        if (ParameterListPrime(token))
        {
            return true;
        }
    }
    
    return false;
}


bool CParser::ParameterListPrime(CLexer& token)
{
    // *** NOT SURE IF THIS IS CORRECT
    
    GetToken(token);
    if ("," == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Parameter List Prime> -> , <Parameter List>", token);
        return ParameterList(token);
    }
    
    SetTokenNeeded(false);
    PrintRule("\t<Parameter List Prime> -> <Empty>\n", token);
    Empty(token);
    return true;
}


bool CParser::Parameter(CLexer& token)
{
    // *** NOT SURE IF THIS IS CORRECT
    PrintRule("\t<Parameter> -> <IDs> : <Qualifier>\n", token);
    
    if (IDs(token))
    {
        // *** already performed token.token in IDs to check if
        //  there was a , (really checked in IDsPrime)
        // ***NEED TO ADD A FIX HERE IN CASE , WAS FOUND NEED TO GET TOKEN
        try
        {
            GetToken(token);
            if (":" == token.GetLexeme())
            {
                SetTokenNeeded(true);
                if(Qualifier(token))
                {
                    return true;
                }
                else
                {
                    SetError(EXPECTED_QUALIFIER, UNKNOWN);
                    throw EXPECTED_QUALIFIER;
                }
            }
            else
            {
                SetError(EXPECTED_SEPARATOR,  COLON);
                throw EXPECTED_SEPARATOR;
            }
        }
        catch(const Error_CParser error)
        {
            PrintError(error, token);
        }
    }
    
    SetTokenNeeded(false);
    return false;
}


bool CParser::Qualifier(CLexer& token)
{
    GetToken(token);
    if ("INT"     == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Qualifier> -> int\n", token);
        return true;
    }
    else if ("BOOLEAN" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Qualifier> -> boolean\n", token);
        return true;
    }
    else if ("REAL"    == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Qualifier> -> real\n", token);
        return true;
    }
    else
    {
        if ("(" == token.GetLexeme())
        {
            SetError(EXPECTED_KEYWORD, UNKNOWN);
            throw GetErrorType();
        }
    }

    SetTokenNeeded(false);
    return false;
}


bool CParser::Body(CLexer& token)
{
    try
    {
        GetToken(token);
        if ("{" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            PrintRule("\t<Body> -> { <Statement List> } \n", token);
            if (StatementList(token))
            {
                GetToken(token);
                
                if ("}" == token.GetLexeme())
                {
                    SetTokenNeeded(true);
                    return true;
                }
                else
                {
                    SetError(EXPECTED_SEPARATOR, RIGHT_BRACE);
                    throw GetErrorType();
                }
            }
            else
            {
                SetError(EXPECTED_STATEMENT, UNKNOWN);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_SEPARATOR, LEFT_BRACE);
            throw GetErrorType();
        }
    } catch (const Error_CParser error)
    {
        PrintError(error, token);
    }
    
    return false;
}


bool CParser::OptDeclarationList(CLexer& token)
{
    PrintRule("\t<Opt Declaration List> -> <Declaration List>", token);
    PrintRule(" | <Empty>\n", token);
    
    //THE ABOVE IS NEEDED IF QUALIFIER AND IDs THROW ERRORS
    if (DeclarationList(token))
    {
        return true;
    }
    else if (Empty(token))
    {
        return true;
    }

    return false;
}


bool CParser::DeclarationList(CLexer& token)
{
    // *** NOT SURE IF THIS IS CORRECT
    PrintRule("\t<Declaration List> -> <Declaration>;", token);
    PrintRule(" <Declaration List Prime>\n", token);
    
    try
    {
        if (Declaration(token))
        {
            GetToken(token);
            if (";" == token.GetLexeme())
            {
                SetTokenNeeded(true);
                return DeclarationListPrime(token);
            }
            else
            {
                SetError(EXPECTED_SEPARATOR, SEMI_COLON);
                throw GetErrorType();
            }
        }
//        else if (Empty(token))
//        {
//            SetTokenNeeded(false);
//            return true;
//        }
    }
    catch(const Error_CParser error)
    {
        PrintError(error, token);
    }
    
    return false;
}


bool CParser::DeclarationListPrime(CLexer& token)
{
    PrintRule("\t<Declaration List Prime> -> <Declaration List> ", token);
    PrintRule(" | <Empty>\n", token);
    
    if (DeclarationList(token))
    {
        return true;
    }
    
    Empty(token);
    return true;
}


bool CParser::Declaration(CLexer& token)
{
    // *** NOT SURE WHAT TO DO HERE
    PrintRule("\t<Declaration> -> <Qualifier> <IDs>\n", token);
    try
    {
        if (Qualifier(token))
        {
            if (IDs(token))
            {
                return true;
            }
            else
            {
                SetError(EXPECTED_IDENTIFIER, UNKNOWN);
                throw GetErrorType();
            }
        }
    }
    catch(const Error_CParser error)
    {
        PrintError(error, token);
    }
    
    return false;
}


bool CParser::IDs(CLexer& token)
{
    GetToken(token);
    if (IDENTIFIER == token.GetTokenType())
    {
        SetTokenNeeded(true);
        PrintRule("\t<IDs> -> <Identifier> <IDs Prime>\n", token);
        return IDsPrime(token);
    }
    
    return false;
}


bool CParser::IDsPrime(CLexer& token)
{
    GetToken(token);
    if ("," == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<IDs Prime> -> , <IDs>\n", token);
        return IDs(token);
    }
    
    SetTokenNeeded(false);
    PrintRule("\t<IDs Prime> -> <Empty>\n", token);
    Empty(token);
    return true;
}


bool CParser::StatementList(CLexer& token)
{
    PrintRule("\t<Statement List> -> <Statement>", token);
    PrintRule(" <Statement List Prime>\n", token);
    
    if (Statement(token) && StatementListPrime(token))
    {
        return true;
    }
    
    return false;
}


bool CParser::StatementListPrime(CLexer& token)
{
    PrintRule("\t<Statement List Prime> -> <Statement List>", token);
    PrintRule(" | <Empty>\n", token);
    
    if (StatementList(token))
    {
        return true;
    }
    
    SetTokenNeeded(false);
    Empty(token);
    return true;
}


bool CParser::Statement(CLexer& token)
{
    GetToken(token);
    if (Compound(token))
    {
        return true;
    }
    else if (Assign(token))
    {
        return true;
    }
    else if (If(token))
    {
        return true;
    }
    else if (Return(token))
    {
        return true;
    }
    else if (Print(token))
    {
        return true;
    }
    else if (Scan(token))
    {
        return true;
    }
    else if (While(token))
    {
        return true;
    }
    else
    {
        if ("(" == token.GetLexeme())
        {
            SetError(EXPECTED_KEYWORD, UNKNOWN);
            throw GetErrorType();
        }
        else if (SEPARATOR == token.GetTokenType() && "}" != token.GetLexeme())
        {
            SetError(EXPECTED_NOTHING, FindErrorExpTokenType(token));
            throw GetErrorType();
        }
        else if (OPERATOR == token.GetTokenType())
        {
            SetError(EXPECTED_IDENTIFIER, FindErrorExpTokenType(token));
            throw GetErrorType();
        }
        else if (INT == token.GetTokenType())
        {
            // needed if you had int number; 0;
            SetError(EXPECTED_IDENTIFIER, FindErrorExpTokenType(token));
            throw GetErrorType();
        }
    }
    
    return false;
}


bool CParser::Compound(CLexer& token)
{
    if ("{" == token.GetLexeme())
    {
        PrintRule("\t<Statement> -> <Compound>\n", token);
        SetTokenNeeded(true);
        PrintRule("\t<Compound> -> { <Statement List> }\n", token);
        if (StatementList(token))
        {
            GetToken(token);
            if ("}" == token.GetLexeme())
            {
                SetTokenNeeded(true);
                return true;
            }
            else
            {
                SetError(EXPECTED_SEPARATOR, RIGHT_BRACE);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_STATEMENT, UNKNOWN);
            throw GetErrorType();
        }
    }

    SetTokenNeeded(false);
    return false;
}


bool CParser::Assign(CLexer& token)
{
    if (IDENTIFIER == token.GetTokenType())
    {
        PrintRule("\t<Statement> -> <Assign>\n", token);
        SetTokenNeeded(true);
        GetToken(token);
        if ("=" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            PrintRule("\t<Assign> -> <Identifier> = <Expression>;\n", token);
            if (Expression(token))
            {
                GetToken(token);
                if (";" == token.GetLexeme())
                {
                    SetTokenNeeded(true);
                    return true;
                }
                else
                {
                    SetError(EXPECTED_OPERATOR, SEMI_COLON);
                    throw GetErrorType();
                }
            }
            else
            {
                SetError(EXPECTED_EXPRESSION, UNKNOWN);
                throw GetErrorType();
            }
        }
        else
        {
            // This will occur if we remove while with an identifier of any
            //  kind. (aka any keyword followed by (
            if ("(" == token.GetLexeme())
            {
                SetError(EXPECTED_KEYWORD, UNKNOWN);
                throw GetErrorType();
            }
            else
            {
                SetError(EXPECTED_OPERATOR, EQUAL);
                throw GetErrorType();
            }
        }
    }
    
    SetTokenNeeded(false);
    return false;
}


bool CParser::If(CLexer& token)
{
    if ("IF" == token.GetLexeme())
    {
        PrintRule("\t<Statement> -> <If>\n", token);
        SetTokenNeeded(true);
        GetToken(token);
        if ("(" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            PrintRule("\t<If> -> if ( <Condition> ) <Statement> <If Prime>", token);
            PrintRule(" endif\n", token);
            if (Condition(token))
            {
                GetToken(token);
                if (")" == token.GetLexeme())
                {
                    SetTokenNeeded(true);
                    if (Statement(token))
                    {
                        IfPrime(token);
                        GetToken(token);
                        if("ENDIF" == token.GetLexeme())
                        {
                            // This is a special case looking for errors if users want
                            //  to put so kind of gibberish behind the endif token like
                            //  ANY SEPARATOR
                            SetTokenNeeded(true);
                            GetToken(token);
                            if ("(" == token.GetLexeme())
                            {
                                SetError(EXPECTED_KEYWORD, UNKNOWN);
                                throw GetErrorType();
                            }
                            else if (SEPARATOR == token.GetTokenType() &&
                                   "{" != token.GetLexeme() && "}" != token.GetLexeme())
                            {
                                SetError(EXPECTED_NOTHING, FindErrorExpTokenType(token));
                                throw GetErrorType();
                            }
                            else if (OPERATOR == token.GetTokenType())
                            {
                                SetError(EXPECTED_NOTHING, TOKEN_FOUND);
                                throw GetErrorType();
                            }
                            else if ("ELSE" == token.GetLexeme())
                            {
                                SetError(EXPECTED_KEYWORD, IF);
                                throw GetErrorType();
                            }
                            else
                            {
                                SetTokenNeeded(false);
                                return true;
                            }
                        }
                        else
                        {
                            SetError(EXPECTED_KEYWORD, ENDIF);
                            throw GetErrorType();
                        }
                    }
                    else
                    {
                        SetError(EXPECTED_STATEMENT, UNKNOWN);
                        throw GetErrorType();
                    }
                }
                else
                {
                    SetError(EXPECTED_SEPARATOR, RIGHT_PAREN);
                    throw GetErrorType();
                }
            }
            else
            {
                SetError(EXPECTED_CONDITION, UNKNOWN);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_SEPARATOR, LEFT_PAREN);
            throw GetErrorType();
        }
    }

    SetTokenNeeded(false);
    return false;
}


bool CParser::IfPrime(CLexer& token)
{
    GetToken(token);
    if("ELSE" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        GetToken(token);
        if ("}" == token.GetLexeme())
        {
            SetError(EXPECTED_SEPARATOR, LEFT_BRACE);
            throw GetErrorType();
        }
        else if (OPERATOR == token.GetTokenType())
        {
            SetError(EXPECTED_NOTHING, TOKEN_FOUND);
            throw GetErrorType();
        }
        else
        {
            SetTokenNeeded(false);
            PrintRule("\t<If Prime> -> else <Statement>\n", token);
            if(Statement(token))
            {
                return true;
            }
            else
            {
                SetError(EXPECTED_STATEMENT, UNKNOWN);
                throw GetErrorType();
            }
        }
    }
    else if (Empty(token))
    {
        SetTokenNeeded(false);
        PrintRule("\t<If Prime> -> <Empty>\n", token);
        return true;
    }
    
    SetTokenNeeded(false);
    return false;
}


bool CParser::Return(CLexer& token)
{
    if ("RETURN" == token.GetLexeme())
    {
        PrintRule("\t<Statement> -> <Return>\n", token);
        SetTokenNeeded(true);
        PrintRule("\t<Return> -> return <Return Prime>;\n", token);
        ReturnPrime(token);
        GetToken(token);
        if (";" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            return true;
        }
        else
        {
            if ( ")" == token.GetLexeme())
            {   // in case a ( was left out
                SetError(EXPECTED_SEPARATOR, LEFT_PAREN);
            }
            else if (INT == token.GetTokenType())
            {   // in case an operator was left out
                SetError(EXPECTED_OPERATOR, UNKNOWN);
            }
            else
            {
                SetError(EXPECTED_SEPARATOR, SEMI_COLON);
            }
            throw GetErrorType();
        }
    }

    SetTokenNeeded(false);
    return false;
}


bool CParser::ReturnPrime(CLexer& token)
{
    PrintRule("\t<Return Prime> -> <Expression> | <Empty>\n", token);
    
    if (Expression(token))
    {
        return true;
    }
    else if (Empty(token))
    {
        return true;
    }
    
    SetTokenNeeded(false);
    return false;
}


bool CParser::Print(CLexer& token)
{
    if ("PUT" == token.GetLexeme())
    {
        PrintRule("\t<Statement> -> <Print>\n", token);
        SetTokenNeeded(true);
        GetToken(token);
        if ("(" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            PrintRule("\t<Print> -> put ( <Expression> );\n", token);
            if (Expression(token))
            {
                GetToken(token);
                if (")" == token.GetLexeme())
                {
                    SetTokenNeeded(true);
                    GetToken(token);
                    if (";" == token.GetLexeme())
                    {
                        SetTokenNeeded(true);
                        return true;
                    }
                    else
                    {
                        SetError(EXPECTED_SEPARATOR, SEMI_COLON);
                        throw GetErrorType();
                    }
                }
                else
                {
                    if (IDENTIFIER == token.GetTokenType())
                    {
                        SetError(EXPECTED_SEPARATOR, LEFT_PAREN);
                        throw GetErrorType();
                    }
                    else
                    {
                        SetError(EXPECTED_SEPARATOR, RIGHT_PAREN);
                        throw GetErrorType();
                    }
                }
            }
            else
            {
                SetError(EXPECTED_EXPRESSION, UNKNOWN);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_SEPARATOR, LEFT_PAREN);
            throw GetErrorType();
        }
    }

    return false;
}


bool CParser::Scan(CLexer& token)
{

    if ("GET" == token.GetLexeme())
    {
        PrintRule("\t<Statement> -> <Scan>\n", token);
        SetTokenNeeded(true);
        GetToken(token);
        if ("(" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            PrintRule("\t<Scan> -> get ( <IDs> );\n", token);
            if(IDs(token))
            {
                GetToken(token);
                if (")" == token.GetLexeme())
                {
                    SetTokenNeeded(true);
                    GetToken(token);
                    if (";" == token.GetLexeme())
                    {
                        SetTokenNeeded(true);
                        return true;
                    }
                    else
                    {
                        SetError(EXPECTED_SEPARATOR, SEMI_COLON);
                        throw GetErrorType();
                    }
                }
                else
                {
                    SetError(EXPECTED_SEPARATOR, RIGHT_PAREN);
                    throw GetErrorType();
                }
            }
            else
            {
                SetError(EXPECTED_IDENTIFIER, UNKNOWN);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_SEPARATOR, LEFT_PAREN);
            throw GetErrorType();
        }
    }
    
    return false;
}


bool CParser::While(CLexer& token)
{
    if ("WHILE" == token.GetLexeme())
    {
        PrintRule("\t<Statement> -> <While>\n", token);
        SetTokenNeeded(true);
        GetToken(token);
        if ("(" == token.GetLexeme())
        {
            SetTokenNeeded(true);
            PrintRule("\t<While> -> while ( <Condition> ) <Statement>\n", token);
            if (Condition(token))
            {
                GetToken(token);
                if (")" == token.GetLexeme())
                {
                    SetTokenNeeded(true);
                    if (Statement(token))
                    {
                        return true;
                    }
                    else
                    {
                        SetError(EXPECTED_STATEMENT, UNKNOWN);
                        throw GetErrorType();
                    }
                }
                else
                {
                    SetError(EXPECTED_SEPARATOR, RIGHT_PAREN);
                    throw GetErrorType();
                }
            }
            else
            {
                SetError(EXPECTED_CONDITION, UNKNOWN);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_SEPARATOR, LEFT_PAREN);
            throw GetErrorType();
        }
    }

    return false;
}


bool CParser::Condition(CLexer& token)
{
    PrintRule("\t<Condition> -> <Expression> <Relop> <Expression>\n", token);
    
    // *** NOT SURE IF THIS IS CORRECT
    try
    {
        if (Expression(token))
        {
            if (Relop(token))
            {
                if (Expression(token))
                {
                    return true;
                }
                else
                {
                    SetError(EXPECTED_EXPRESSION, UNKNOWN);
                    throw GetErrorType();
                }
            }
            else
            {
                SetError(EXPECTED_RELOP, UNKNOWN);
                throw GetErrorType();
            }
        }
        else
        {
            if (")" == token.GetLexeme())
            {
                SetError(EXPECTED_CONDITION, UNKNOWN);
                throw GetErrorType();
            }
            else
            {
                SetError(EXPECTED_EXPRESSION, UNKNOWN);
                throw GetErrorType();
            }
        }
    }
    catch(const Error_CParser error)
    {
        PrintError(error, token);
    }
    
    return false;
}


bool CParser::Relop(CLexer& token)
{
    GetToken(token);
    std::string lexeme = token.GetLexeme();
    if ("==" == lexeme)
    {
        SetTokenNeeded(true);
        PrintRule("\t<Relop> -> ==\n", token);
        return true;
    }
    else if ("^=" == lexeme)
    {
        SetTokenNeeded(true);
        PrintRule("\t<Relop> -> ^=\n", token);
        return true;
    }
    else if (">" == lexeme)
    {
        SetTokenNeeded(true);
        PrintRule("\t<Relop> -> >\n", token);
        return true;
    }
    else if ("<" == lexeme)
    {
        SetTokenNeeded(true);
        PrintRule("\t<Relop> -> <\n", token);
        return true;
    }
    else if ("=>" == lexeme)
    {
        SetTokenNeeded(true);
        PrintRule("\t<Relop> -> =>\n", token);
        return true;
    }
    else if ("=<" == lexeme)
    {
        SetTokenNeeded(true);
        PrintRule("\t<Relop> -> =<\n", token);
        return true;
    }
    else
    {
        SetError(EXPECTED_RELOP, UNKNOWN);
        throw GetErrorType();
    }
        
    return false;
}


bool CParser::Expression(CLexer& token)
{
    PrintRule("\t<Expression> -> <Term> <Expression Prime>\n", token);
    
    if (Term(token) && ExpressionPrime(token))
    {
        return true;
    }
    
    return false;
}


bool CParser::ExpressionPrime(CLexer& token)
{
    GetToken(token);
    if ("+" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Expression Prime> -> + <Term> <Expression Prime>\n", token);
        if (Term(token) && ExpressionPrime(token))
        {
            return true;
        }
    }
    else if ("-" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Expression Prime> -> - <Term> <Expression Prime>\n", token);
        if (Term(token) && ExpressionPrime(token))
        {
            return true;
        }
    }
    
    SetTokenNeeded(false);
    PrintRule("\t<Expression Prime> -> <Empty>\n", token);
    Empty(token);
    return true;

}


bool CParser::Term(CLexer& token)
{
    PrintRule("\t<Term> -> <Factor> <Term Prime>\n", token);
    
    if (Factor(token) && TermPrime(token))
    {
        return true;
    }
    
    return false;
}


bool CParser::TermPrime(CLexer& token)
{
    GetToken(token);
    if ("*" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Term Prime> -> * <Factor> <Term Prime>\n", token);
        Factor(token);
        TermPrime(token);
        return true;
    }
    else if (("/" == token.GetLexeme()))
    {
        SetTokenNeeded(true);
        PrintRule("\t<Term Prime> -> / <Factor> <Term Prime>\n", token);
        Factor(token);
        TermPrime(token);
        return true;
    }
    
    SetTokenNeeded(false);
    PrintRule("\t<Term Prime> -> <Empty>\n", token);
    Empty(token);
    return true;
}


bool CParser::Factor(CLexer& token)
{
    PrintRule("\t<Factor> -> - <Primary> | <Primary>\n", token);
    
    GetToken(token);
    if ("-" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        GetToken(token);
        return Primary(token);
    }
    else
    {
        SetTokenNeeded(false);
        return Primary(token);
    }
    
    return false;
}


bool CParser::Primary(CLexer& token)
{
    // we already called GetToken(token) in Factor function
    //  because we had to check if there was a - or not so we
    //  dont do it here
    if (IDENTIFIER == token.GetTokenType())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Primary> -> <Identifier> <Identifier Prime>\n", token);
        return IdentifierPrime(token);
    }
    else if (INT == token.GetTokenType())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Primary> -> <Integer>\n", token);
        return true;
    }
    else if ("(" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Primary> -> ( <Expression> )\n", token);
        if (Expression(token))
        {
            GetToken(token);
            if (")" == token.GetLexeme())
            {
                SetTokenNeeded(true);
                return true;
            }
            else
            {
                SetError(EXPECTED_SEPARATOR, RIGHT_PAREN);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_EXPRESSION, UNKNOWN);
            throw GetErrorType();
        }
    }
    else if (REAL == token.GetTokenType())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Primary> -> <Real>\n", token);
        return true;
    }
    else if ("TRUE" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Primary> -> true\n", token);
        return true;
    }
    else if ("FALSE" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Primary> -> false\n", token);
        return true;
    }
    
    return false;
}


bool CParser::IdentifierPrime(CLexer& token)
{
    GetToken(token);
    if ("(" == token.GetLexeme())
    {
        SetTokenNeeded(true);
        PrintRule("\t<Identifier Prime> -> ( <IDs> )\n", token);
        if (IDs(token))
        {
            GetToken(token);
            if (")" == token.GetLexeme())
            {
                SetTokenNeeded(true);
                return true;
            }
            else
            {
                SetError(EXPECTED_SEPARATOR, RIGHT_PAREN);
                throw GetErrorType();
            }
        }
        else
        {
            SetError(EXPECTED_IDENTIFIER, UNKNOWN);
            throw GetErrorType();
        }
    }
    
    PrintRule("\t<Identifier Prime> -> <Empty>\n", token);
    Empty(token);
    SetTokenNeeded(false);
    return true;
}


bool CParser::Empty(CLexer& token)
{
    PrintRule("\t<Empty> -> epsilon\n", token);
    return true;
}


//=============================== HELP FUNTIONS ================================
//=============================== HELP FUNTIONS ================================
//=============================== HELP FUNTIONS ================================
//=============================== HELP FUNTIONS ================================
void CParser::GetToken(CLexer& token)
{
    if (TokenNeeded())
    {
        // if the token has been used then get a new one
        try
        {
            token.Lexer();
        }
        catch(const Error_CLexer error)
        {
            SetErrorThrown(true);
        }
        
        if (!ErrorThrown() && TYPE_ERROR != token.GetTokenType())
        {
            PrintToken(token);
        }
    }
}


void CParser::SetError(const Error_CParser error, const Exp_Token_Type expToken)
{
    //***IMPORTANT NOTE SetErrorExpTokenType must be called before SetErrorType
    SetErrorExpTokenType(expToken);
    SetErrorType(error);
}


Exp_Token_Type CParser::FindErrorExpTokenType(const CLexer& token)
{
    if ("%%" == token.GetLexeme())
    {
        return PERCENT;
    }
    else if ("FUNCTION" == token.GetLexeme())
    {
        return FUNCTION;
    }
    else if ("[" == token.GetLexeme())
    {
        return LEFT_BRACKET;
    }
    else if ("]" == token.GetLexeme())
    {
        return RIGHT_BRACKET;
    }
    else if ("," == token.GetLexeme())
    {
        return COMMA;
    }
    else if (":" == token.GetLexeme())
    {
        return COLON;
    }
    else if (";" == token.GetLexeme())
    {
        return SEMI_COLON;
    }
    else if ("{" == token.GetLexeme())
    {
        return LEFT_BRACE;
    }
    else if ("}" == token.GetLexeme())
    {
        return RIGHT_BRACE;
    }
    else if ("=" == token.GetLexeme())
    {
        return EQUAL;
    }
    else if ("IF" == token.GetLexeme())
    {
        return IF;
    }
    else if ("(" == token.GetLexeme())
    {
        return LEFT_PAREN;
    }
    else if (")" == token.GetLexeme())
    {
        return RIGHT_PAREN;
    }
    else if ("ENDIF" == token.GetLexeme())
    {
        return ENDIF;
    }
    else if ("RETURN" == token.GetLexeme())
    {
        return RETURN;
    }
    else if ("PUT" == token.GetLexeme())
    {
        return PUT;
    }
    else if ("GET" == token.GetLexeme())
    {
        return GET;
    }
    else if ("WHILE" == token.GetLexeme())
    {
        return WHILE;
    }
    
    return UNKNOWN;
}


void CParser::PrintExpTokenType(const CLexer& token)
{
    switch(GetErrorExpTokenType())
    {
        case TOKEN_FOUND:
            std::cout << "\"" << token.GetLexeme() << "\"";
            break;
            
        case PERCENT:
            std::cout << "\"%%\"";
            break;
            
        case FUNCTION:
            std::cout << "\"function\"";
            break;
            
        case LEFT_BRACKET:
            std::cout << "\"[\"";
            break;
            
        case RIGHT_BRACKET:
            std::cout << "\"]\"";
            break;
            
        case COMMA:
            std::cout << "\",\"";
            break;

        case COLON:
            std::cout << "\":\"";
            break;
            
        case SEMI_COLON:
            std::cout << "\";\"";
            break;
            
        case LEFT_BRACE:
            std::cout << "\"{\"";
            break;
            
        case RIGHT_BRACE:
            std::cout << "\"}\"";
            break;
            
        case EQUAL:
            std::cout << "\"=\"";
            break;
            
        case IF:
            std::cout << "\"if\"";
            break;
            
        case LEFT_PAREN:
            std::cout << "\"(\"";
            break;
            
        case RIGHT_PAREN:
            std::cout << "\")\"";
            break;
            
        case ENDIF:
            std::cout << "\"endif\"";
            break;
            
        case RETURN:
            std::cout << "\"return\"";
            break;
            
        case PUT:
            std::cout << "\"put\"";
            break;
            
        case GET:
            std::cout << "\"get\"";
            break;
            
        case WHILE:
            std::cout << "\"while\"";
            break;
        
        case UNKNOWN:
        default:
            std::cout << "";
            break;
    }
}


void CParser::PrintError(const Error_CParser error, CLexer& token)
{

    long long colmNum = 0;
    if (token.GetColmNum() > 0)
    {
        colmNum = token.GetColmNum() - token.CalcErrorOffset();
    }
    
    if (!ErrorThrown())
    {
        std::cout << std::endl;
        
        switch(error)
        {
            case EXPECTED_CONDITION:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected condition " << std::endl;
                break;
                
            case EXPECTED_EXPRESSION:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected expression " << std::endl;
                PrintToken(token);
                break;
                
            case EXPECTED_IDENTIFIER:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected identifier " << std::endl;
                PrintToken(token);
                break;
                
            case EXPECTED_KEYWORD:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected keyword "; PrintExpTokenType(token);
                std::cout << std::endl << "\t";      PrintToken(token);
                break;
                
            case EXPECTED_NOTHING:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": unneeded "; token.PrintTokenType();
                std::cout << " "; PrintExpTokenType(token);
                std::cout << std::endl << "\t";
                PrintToken(token);
                break;
                
            case EXPECTED_OPERATOR:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected operator " << std::endl;
                std::cout << "\t";
                PrintToken(token);
                break;
                
            case EXPECTED_RELOP:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected relational operator " << std::endl;
                PrintToken(token);
                break;
                
            case EXPECTED_SEPARATOR:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected separator "; PrintExpTokenType(token);
                std::cout << std::endl << "\t";
                PrintToken(token);
                break;
            
            case EXPECTED_STATEMENT:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected statement " << std::endl;
                std::cout << "\t";
                PrintToken(token);
                break;
                
            case EXPECTED_QUALIFIER:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": expected qualifier " << std::endl;
                PrintToken(token);
                break;
                
            default:
                std::cout << "RAT18S error: " << token.GetLineNum() << ":"
                << colmNum << ": unknown error occured " << std::endl;
                std::cout << "\t";
                PrintToken(token);
                break;
        }
    }
    
    // doesnt allow other output to display but allows program to exit correctly
    SetErrorThrown(true);
}

void CParser::PrintRule(const std::string output, const CLexer& token)
{
    if (PRINT_RULE_STATEMENTS)
    {
        if (!ErrorThrown())
        {
            std::cout << output;
        }
    }
}


void CParser::PrintToken(const CLexer& token)
{
    std::cout << "Token: ";
    
    switch(token.GetTokenType())
    {
        case IDENTIFIER:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout <<  "Identifier";
            break;
            
        case KEYWORD:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout << "Keyword";
            break;
        case INT:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout << "Integer";
            break;
            
        case REAL:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout << "Real";
            break;
            
        case OPERATOR:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout << "Operator";
            break;
            
        case SEPARATOR:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout << "Separator";
            break;
            
        case SYMBOL:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout << "Symbol";
            break;
            
        default:
            std::cout << std::left << std::setw(strlen("identifier"));
            std::cout << "Unknown";
            break;
    }
    
    if (token.GetLexeme() == "")
    {
        std::cout << "\t Lexeme: EOF" << "\n";
    }
    else
    {
        std::cout << "\t Lexeme: " << token.GetLexeme() << "\n";
    }
}




