// Date: 2/11/18
// Author: Austin Blanke
// Class: CPSC 323 Compilers & Languages
// File: CLexer.cpp

#include <cctype>
#include <fstream>
#include <iostream>
#include "CLexer.h"

// ============================= CToken  Start =================================
//==============================================================================
// Description: This function will print out the token type to the screen.
//
// Input: NONE
//
// Output: NONE
//
//==============================================================================
void CToken::PrintTokenType() const
{
    switch(m_tokenType)
    {
        case IDENTIFIER:
            std::cout <<  "identifier";
            break;
            
        case KEYWORD:
            std::cout << "keyword";
            break;
        case INT_VALUE:
            std::cout << "int";
            break;
            
        case REAL_VALUE:
            std::cout << "real";
            break;
            
        case OPERATOR:
            std::cout << "operator";
            break;
            
        case SEPARATOR:
            std::cout << "separator";
            break;
            
        case SYMBOL:
            std::cout << "symbol";
            break;
            
        default:
            std::cout << "unknown";
            break;
    }
}



// ============================= CLexer  Start =================================
//==============================================================================
// Description: This function will return the front of a tokens place in the
//              input file.
//
// Input: NONE
//
// Output: NONE
//
//==============================================================================
long long CLexer::CalcErrorOffset() const
{
    long long colmOffset = 0;
    
    if (m_token.GetLexemeLength() > 1)
    {
        colmOffset = m_token.GetLexemeLength()-1;
    }
    else
    {
        colmOffset = 0;
    }
    
    return colmOffset;
}


//==============================================================================
// Description: This function will capitalize a character if lowercase letter.
//
// Input: curChar - the char we want to upper case if possible.
//
// Output: NONE
//
//==============================================================================
void CLexer::CapitalizeChar(char& curChar)
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
//        Machine) state if the current character is apart of a legal
//        identifier else it will throw an ERROR type.
//
// Input: curChar - the current character taken from the source file for
//            evaluation.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//       false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool CLexer::CheckIdentifier(const char curChar)
{
    State curState = GetCurrentState();
    
    if (std::isalpha(curChar) && (curState == INITIAL_STATE
                                  || curState == INSIDE_IDENTIFIER || curState == COULD_END_IDENTIFIER))
    {
        // identifiers must lead with a letter or end with a
        // letter with the exception of ending with a $
        SetCurrentState(COULD_END_IDENTIFIER);
        return true;
        
    }
    else if(curChar == '$' && (curState == INSIDE_IDENTIFIER || curState == COULD_END_IDENTIFIER))
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
//        not. If the token is a keyword the current State of the FSM
//        (Finite State Machine) will be updated accordingly.
//
// Input: curChar - the current character taken from the source file for
//            evaluation.
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//       false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool CLexer::CheckKeyword(const char curChar)
{
    // stores the lexeme
    std::string lexeme = GetLexeme();
    
    if (IsSeparator(curChar))
    {
        // remove a seperator from a possible keyword like get( or put(
        lexeme.pop_back();
    }
    
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
            SetTokenType(KEYWORD);
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
//        Machine) state if the current character is apart of a legal
//        number, such as an integer or real, else it will issue an error.
//
// Input: curChar - the current character taken from the source file for
//            evaluation.
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//       false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool CLexer::CheckNumber(const char curChar)
{
    bool isDigit = std::isdigit(curChar);
    State curState = GetCurrentState();
    
    if (isDigit && curState == INITIAL_STATE)
    {
        // found a number so we are inside a number
        SetCurrentState(INSIDE_INTEGER);
        return true;
    }
    else if (isDigit && curState == INSIDE_INTEGER)
    {
        return true;
    }
    else if (curChar == '.')
    {
        if (curState == INSIDE_INTEGER)
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
//        Machine) state if the current character is apart of a legal
//        operator else it will issue an error.
//
// Input: curChar - the current character taken from the source file for
//            evaluation.
//
// Output: true - if the state of the FSM (Finite State Machine) was changed.
//       false - if the state of the FSM (Finite State Machine) was unchanged.
//
//==============================================================================
bool CLexer::CheckOperators(const char curChar)
{
    State curState = GetCurrentState();
    
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
                SetTokenType(OPERATOR);
                return true;
            }
            else
            {    // invalid symbol found in wrong state
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
            // < operator is valid and so is =< or =>
            if (curState == INITIAL_STATE || curState == EQUAL_OPERATOR)
            {
                SetCurrentState(END_OPERATOR);
                SetTokenType(OPERATOR);
                return true;
            }
            else
            {
                throw DetermineError();
            }
            
            break;
            
        case '=':
            if (curState == INITIAL_STATE)
            {
                // found first portion of ==, =, or =<
                SetCurrentState(EQUAL_OPERATOR);
                return true;
            }
            else if (curState == CARROT_OPERATOR || curState == EQUAL_OPERATOR || curState == POTENTIAL_OPERATOR)
            {
                // found the ^= operator
                SetCurrentState(END_OPERATOR);
                SetTokenType(OPERATOR);
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
//        Machine) state if the current character is apart of a legal
//        separator else it will issue an error.
//
// Input: curChar - the current character taken from the source file for
//            evaluation.
//
// Output: true - if the state of the Finite State Machine is changed (updated).
//       false - if the state of the Finite State Machine is unchanged.
//
//==============================================================================
bool CLexer::CheckSeparators(const char curChar)
{
    State curState = GetCurrentState();
    
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
            // dont care for matching () {} just care that we notice its a
            //  separator
            SetCurrentState(END_SEPARATOR);
            SetTokenType(SEPARATOR);
            return true;
        }
    }
    else if (curChar == '%' && curState == PERCENT_SEPARATOR)
    {
        // found the closing part of a separator character
        SetCurrentState(END_SEPARATOR);
        SetTokenType(SEPARATOR);
        return true;
    }
    
    return false;
}



//==============================================================================
// Description: This function will determine an error given the current state
//        of the FSM (Finite State Machine). It will then return the
//        error as an enum ERROR type.
//
// Input: NONE
//
// Output: Error - returns an enum error value to the corresponding error that
//           occured.
//
//==============================================================================
Error_CLexer CLexer::DetermineError()
{
    switch(GetCurrentState())
    {
            // ERROR CASES FOR IDENTIFIERS
        case INSIDE_IDENTIFIER:
        case COULD_END_IDENTIFIER:
        case DOLLAR_IDENTIFIER:
            return INVALID_IDENTIFIER;
            
            // ERROR CASES FOR NUMBERS
        case INSIDE_INTEGER:
            return INVALID_INTEGER;
            
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
    return CLEXER_UNKNOWN_ERROR;
}



//==============================================================================
// Description: This function will check the given character recieved from the
//        source file and compare it with the current FSM (Finite State
//        Machine) and change the state if it determines the end of a
//        token was found. If the FSM (Finite State Machine) is will throw
//        an ERROR type.
//
// Input: curChar - the current character taken from the source file for
//            evaluation.
//
//        endFile - is true if it is the end of the file
//
// Output: true - End of a token has been found.
//       false - End of a token has not been found.
//
//==============================================================================
bool CLexer::EndOfToken(const char curChar, const bool endFile)
{
    
    if (IsWhiteSpace(curChar) || IsSeparator(curChar) || IsOperator(curChar)
        || IsComment(curChar) || endFile)
    {
        switch(GetCurrentState())
        {
                // cases for IDENTIFIERS
            case COULD_END_IDENTIFIER:
            case DOLLAR_IDENTIFIER:
                // we have just found out that it is an
                //  identifier so label as accepted
            
                // check if the identifier is really a keyword
                if (!CheckKeyword(curChar))
                {
                    SetCurrentState(END_IDENTIFIER);
                    SetTokenType(IDENTIFIER);
                }
            
                return true;
                break;
                
                // cases for INTEGERS
            case INSIDE_INTEGER:
                // we have just found out that its is not a
                //  number anymore so we label it as accepted
                SetCurrentState(END_INTEGER);
                SetTokenType(INT_VALUE);
                return true;
                break;
                
            case COULD_END_REAL:
                // we just found out that it is a real number
                //  so we label it as an accepting state
                SetCurrentState(END_REAL);
                SetTokenType(REAL_VALUE);
                return true;
                break;
                
                // cases for OPERATORS
                // not all operators need to be here
                
            case POTENTIAL_OPERATOR:
                // we have just found a > and IT MUST BE A SPACE
                // or we found >= need to throw error for <=
                if (IsWhiteSpace(curChar) || curChar == '=')
                {
                    SetCurrentState(END_OPERATOR);
                    SetTokenType(OPERATOR);
                    return true;
                }
                else if (IsSeparator(curChar) || IsComment(curChar))
                {
                    // need to unget character from > such as when
                    // >[ or >( or >%% or >;
                    return true;
                }
                else
                {
                    throw DetermineError();
                }
                
                break;
                
            case EQUAL_OPERATOR:
                
                // we have just found =, =<
                // if it is =' ' or == then legal operator
                if (IsWhiteSpace(curChar) || curChar == '='
                    || curChar == '<' || curChar == '>')
                {
                    SetCurrentState(END_OPERATOR);
                    SetTokenType(OPERATOR);
                    return true;
                    
                }
                else
                {
                    // we have to truncate token such as => into = and >
                    return true;
                }
                
                
                break;
                
                // cases for OTHER
            case INITIAL_STATE:
                if (IsOperator(curChar)) return false;
                else if (IsWhiteSpace(curChar)) return false;
                else if (IsSeparator(curChar)) return false; //needs to be checked further
                return false;
                
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
//        functionality. This function will return true if the state of
//        the FSM (Finite State Machine) was updated/changed or false
//        if the state was unchanged.
//
// Input: curChar - the current char recieved from the input file
//
// Output: NONE
//
//==============================================================================
void CLexer::FSM(const char curChar)
{
    State curState = GetCurrentState();
    
    if (IsWhiteSpace(curChar) && curState == INITIAL_STATE)
    {}
    // run every machines apart of the FSM
    else if (CheckIdentifier(curChar) || CheckNumber(curChar) || CheckOperators(curChar) ||
             CheckSeparators(curChar))
    {}
    else
    {
        // check if invalid symbol found on end of a token
        Error_CLexer error = DetermineError();
        
        if ((error == INVALID_IDENTIFIER && curState == COULD_END_IDENTIFIER)
            || isalpha(curChar))
        {
            // remove the invalid symbol after the identifier
            RemoveLastCharLexeme();
            // we have found a identifier token
            SetCurrentState(END_IDENTIFIER);
            SetTokenType(IDENTIFIER);
            // move the file pointer pointing back at the invalid symbol
            m_inputFile.unget();
        }
        else
        {
            // separator was found in an invalid state
            throw error;
        }
    }
}



//==============================================================================
// Description: This function will incremement the column number and line
//              number if needed.
//
// Input:       curChar - the current character pulled from the input file.
//
// Output: NONE
//
//==============================================================================
void CLexer::IncrementFileCounters(const char curChar)
{
    if (curChar == '\n' || curChar == '\0')
    {
        SetLineNum(GetLineNum()+1);
    }
    else
    {
        SetColmNum(GetColmNum()+1);
    }
}



//==============================================================================
// Description: This function will return one token from the source file each
//              call.
// Input: NONE
// Output: Returns the token found.
//==============================================================================
CToken CLexer::Lexer()
{
    SetTokenType(TYPE_ERROR);       // make sure there is not old token type
    ClearLexeme();            // make sure there are no old lexemes
    bool tokenFound = false;    // if token is found set true
    char fileChar;            // stores the chars extracted from file
    bool endOfFile = false;     // determines if end of file is reached
    
    // look for a token until token is found or end of source file is found
    do
    {
        // retrieve a character from the file white fileChar is not whitespace
        m_inputFile.get(fileChar);
        
        endOfFile = m_inputFile.eof();
        
        IncrementFileCounters(fileChar);
        
        // Have the FSM find initial comment character and then
        //  this will catch the last comment character
        if (IsComment(fileChar))
        {
            // store the current line and column number encase we encounter
            //  eof and need to display an error
            long long curLine = GetLineNum();
            long long curColm = GetColmNum();
            
            do
            {
                m_inputFile.get(fileChar);
                IncrementFileCounters(fileChar);
                endOfFile = m_inputFile.eof();
                if (endOfFile)
                {
                    // set the line/column number to what it was before we
                    //  encountered eof
                    SetLineNum(curLine);
                    SetColmNum(curColm);
                    // adjust counters by what we last char found
                    IncrementFileCounters(fileChar);
                    break;
                }
            } while (!IsComment(fileChar));
            
            //If we found another comment
            if (IsComment(fileChar) && !endOfFile)
            {
                //get another character if not eof
                m_inputFile.get(fileChar);
                IncrementFileCounters(fileChar);
            }
            
        }
        
        
        // append the fileChar to the token
        if (!IsWhiteSpace(fileChar))
        {
            // capitalize the character from the file because RAT18S is
            //  insensative to upper case or lower case names
            CapitalizeChar(fileChar);
            AppendToLexeme(fileChar);
        }
        
        try
        {
            // if operator or separator is encounters we need
            //  to back up the file pointer. However we dont
            //  need to move file pointer if it is whitespace
            if (EndOfToken(fileChar, endOfFile))
            {
                // dont push file pointer back for comment/string literal or whitespace
                if (!IsComment(fileChar) || (IsComment(fileChar) && GetCurrentState() == POTENTIAL_OPERATOR))
                {
                    if (!IsWhiteSpace(fileChar) && GetCurrentState() != END_OPERATOR)
                    {
                        // push the file pointer back
                        m_inputFile.unget();
                        SetColmNum(GetColmNum()-1);    // decrease column number
                        RemoveLastCharLexeme();
                        
                        // found a = operator but had to truncate whatever was
                        //  behind it
                        if (GetCurrentState() == EQUAL_OPERATOR ||
                            GetCurrentState() == POTENTIAL_OPERATOR)
                        {
                            SetCurrentState(END_OPERATOR);
                            SetTokenType(OPERATOR);
                        }
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
        catch(const Error_CLexer error)
        {
            bool foundNewToken = false;
            
            // if its an invalid symbol then we just remove the first character
            //  ex. @hi want to print @ as error and print identifier hi
            if (error == INVALID_SYMBOL)
            {
                if (IsWhiteSpace(fileChar))
                {
                    // set the colm number back one to get front of
                    //  invalid symbol
                    SetColmNum(GetColmNum()-1);
                }
                
                PrintError(error);
                
                RemoveLastCharLexeme();
                
                if (IsWhiteSpace(fileChar))
                {
                    // move column number where it should be
                    SetColmNum(GetColmNum()+1);
                }
                
                // reset the FSM state
                SetCurrentState(INITIAL_STATE);
                
                //clear current token weve been storing
                ClearLexeme();
            }
            // if we have an invalid symbol after a legal identifier then we
            //  need to return the token and print invalid symbol (dont believe
            //  this is correct way to do it because we would have to call the
            //  lexer again to print error but then again it doesnt really matter
            //  because the next time lexer is called then it will print the
            //  same error and then try to find the next token
            else
            {
                
                // while we have found an error we need to find next token
                while(!IsWhiteSpace(fileChar) && !IsOperator(fileChar) &&
                      !IsSeparator(fileChar) && !IsComment(fileChar) &&
                      !endOfFile)
                {
                    foundNewToken = true;   // we had to search for new token
                    m_inputFile.get(fileChar);
                    endOfFile = m_inputFile.eof();
                    
                    // append all characters associated with error
                    AppendToLexeme(fileChar);
                    
                    if (!IsWhiteSpace(fileChar))
                    {
                        // if we encounter whitespace/newlines incrememnt file
                        //  counters accordingly
                        IncrementFileCounters(fileChar);
                    }
                }
                
                // remove last character associated with new token as not part
                //  of error
                if (foundNewToken)
                {
                    RemoveLastCharLexeme();
                    
                    
                    // we found our next token so put it back and reset the FSM
                    //  state
                    if (!IsWhiteSpace(fileChar))
                    {   // if not whitespace then we need to store that token
                        m_inputFile.unget();
                    }
                }
                else if (IsWhiteSpace(fileChar) && !foundNewToken)
                {
                    // if we encountered an error but its a single character
                    if (fileChar == '\n' || fileChar == '\0')
                    {
                        SetLineNum(GetLineNum()-1);
                    }
                    else
                    {
                        SetColmNum(GetColmNum()-1);
                    }
                }
                else if (IsSeparator(fileChar))
                {
                    RemoveLastCharLexeme();
                    m_inputFile.unget();
                    SetColmNum(GetColmNum()-1);
                }
                
                // print the error to the user
                PrintError(error);
                
                if (IsWhiteSpace(fileChar))
                {
                    // if we encounter whitespace/newlines incrememnt file
                    //  counters accordingly
                    IncrementFileCounters(fileChar);
                }
                
                SetCurrentState(INITIAL_STATE);
                
                //clear current token weve been storing
                ClearLexeme();
            }
        }
        
    } while (!tokenFound && !endOfFile);
    
    return m_token;
}



//==============================================================================
// Description: This function will determine if the current state of the FSM
//        (Finite State Machine) is in an accepting state or not.
//
// Input: NONE
//
// Output: true - the current state of the FSM (Finite State Machine) is in an
//          accepting state.
//       false - the current state of the FSM (Finite State Machine) is not
//           in an accepting state.
//
//==============================================================================
bool CLexer::IsInAcceptingState()
{
    switch(m_fsmState)
    {
        case END_IDENTIFIER:
        case END_INTEGER:
        case END_REAL:
        case END_KEYWORD:
        case END_OPERATOR:
        case END_SEPARATOR:
            //        case END_COMMENT:
            SetCurrentState(INITIAL_STATE);    // reset the FSM
            return true;    // an accepting state
            
            // special cases where the FSM is not reset
            //        case INSIDE_COMMENT:
            //            return true;
            
        default:
            return false;    // not an accepting state
    }
    
    return false;
}



//==============================================================================
// Description: This function will determine if the char passed by argument is
//        a legal comment character in the RAT18S language.
//
// Input: curChar - the current char we are testing to determine if its a
//            legal comment character.
//
// Output: true - curChar is a legal comment character for RAT18S.
//       false - curChar is an illegal comment char for RAT18S.
//
//==============================================================================
bool CLexer::IsComment(const char curChar)
{
    return curChar == '!';
}


//==============================================================================
// Description: This function will determine if the char passed by argument is
//        a legal beginning of an operator only in the RAT18S language.
//
// Input: curChar - the current char we are testing to determine if its a
//            legal beginning operator character.
//
// Output: true - curChar is a legal beginning operator character for RAT18S.
//       false - curChar is an illegal beginning operator char for RAT18S.
//
//==============================================================================
bool CLexer::IsOperator(const char curChar)
{
    for (int i = 0; i < OPERATOR_AMOUNT; i++)
    {
        if (curChar == OPERATORS[i])    return true;
    }
    return false;
}



//==============================================================================
// Description: This function will determine if the char passed by argument is
//        a legal separator in the RAT18S language.
//
// Input: curChar - the current char we are testing to determine if its a
//            legal separator character.
//
// Output: true - curChar is a legal separator character for RAT18S.
//       false - curChar is an illegal separator character for RAT18S.
//
//==============================================================================
bool CLexer::IsSeparator(const char curChar)
{
    for (int i = 0; i < SEPARATOR_AMOUNT; i++)
    {
        if (curChar == SEPARATORS[i])    return true;
    }
    
    return false;
}



//==============================================================================
// Description: This function will determine if the char passed by argument is
//              a legal whitespace character in the RAT18S language.
//
// Input: curChar - the current char we are testing to determine if its a
//            legal whitespace character.
//
// Output: true - curChar is a legal whitespace character for RAT18S.
//       false - curChar is an illegal whitespace character for RAT18S.
//
//==============================================================================
bool CLexer::IsWhiteSpace(const char curChar)
{
    for (int i = 0; i < WHITESPACE_AMOUNT; i++)
    {
        if (curChar == WHITESPACES[i])    return true;
    }
    
    return false;
}



//==============================================================================
// Description: This function will print out a given Error along with the
//        possible token we believe is in error as well as what line
//        number and column number this error can be found.
//
// Input:    errorType - the type of error that was caused
//
// Output: NONE
//
//==============================================================================
void CLexer::PrintError(const Error_CLexer errorType)
{
    std::cout << std::endl;
    
    // gets the front of an error if its not a single character
    long long colmOffset = CalcErrorOffset();
    std::cout << GetLineNum() << ":" << GetColmNum()-colmOffset
              << " RAT18S error:";
    
    switch(errorType)
    {
        case INVALID_IDENTIFIER:
            std::cout << " invalid identifier \"" << GetLexeme() << "\""
                      << std::endl;
            throw INVALID_IDENTIFIER;
            break;
            
        case INVALID_INTEGER:
            std::cout << " invalid integer \"" << GetLexeme() << "\""
                      << std::endl;
            throw INVALID_INTEGER;
            break;
            
        case INVALID_REAL:
            std::cout << " invalid real \"" << GetLexeme() << "\"" << std::endl;
            throw INVALID_REAL;
            break;
            
        case INVALID_OPERATOR:
            std::cout << " invalid operator \"" << GetLexeme() << "\""
                      << std::endl;
            throw INVALID_OPERATOR;
            break;
            
        case INVALID_SYMBOL:
            std::cout << " invalid symbol \"" << GetLexeme() << "\""
                      << std::endl;
            throw INVALID_SYMBOL;
            break;
            
        default:
            // unknown error
            std::cout << " unknown error occured \"" << GetLexeme() << "\""
                      << std::endl;
            throw CLEXER_UNKNOWN_ERROR;
            break;
    }
}


// ========================== Lexical Analysis End =============================




