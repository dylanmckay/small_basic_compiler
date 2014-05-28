#include "../../../include/epsilon/parser/Tokenizer.hpp"

#include <cctype>
#include <stdexcept>

using namespace epsilon;
using namespace parser;


/*!
 * Checks if the given character is a valid first character for an identifier.
 */
bool isWord(char firstChar)
{
    return std::isalpha(firstChar) || (firstChar == '_');
}

/*!
 * Checks if the given character can appear at any point except the first character in an identifer.
 */
bool canBeInWord(char c)
{
    return isWord(c) || std::isdigit(c);
}

Token Tokenizer::readNextToken()
{
    //
    if(isEndOfStream() && !m_firstEOF)
    {
        m_firstEOF = true;

        return Token::GetNewLine(m_file, m_line, m_column);
    }
    else if(isEndOfStream())
        return Token::Null;

    // Check if we've started reading yet.
    if(!m_started)
    {
        m_started = true;

        if(!readChar(false))
            return Token::Null;
    }

    // Attempt to read whitespace.
    if(eatWhitespace()) // An EOL or EOF was reached.
    {
        return Token::Token::GetNewLine(m_file, m_line, m_column);
    }

    if(isEndOfStream())
        return Token::Null;

    // Check if the current character is the start of a comment.
    if(m_currentChar == '\'')
    {
        while(1)
        {
            if(readChar(false))
            {
                if(m_currentChar == '\n')
                {
                    m_line++;
                    readChar(false); // Eat the new line.
                    return Token::GetNewLine(m_file, m_line, m_column);
                }
            }
            else
                break;
        }
    }

    if(isEndOfStream())
        return Token::Null;


    if(isWord(m_currentChar)) // Check for word.
    {
        return readWord();
    }
    else if(::isdigit(m_currentChar)) // Check for numeric literal.
    {
        // TODO: check for negative.
        return readNumericLiteral();
    }
    else if(m_currentChar == '\"') // Check for string constant.
    {
        return readStringLiteral();
    }
    else   // It is either a symbol or an invalid token.
    {
        return readSymbol();
    }
}

bool Tokenizer::eatWhitespace()
{
    // Check if there's no space to eat.
    if(!std::isspace(m_currentChar))
        return false;

    // While whitespace.
    while(std::isspace(m_currentChar))
    {
        if(m_currentChar == '\n')
        {
            m_line++;
            readChar(false); // Eat the new line.
            return true;
        }

        if(!readChar(false))
        {
            return true;
        }

    }

    return false;
}


Token Tokenizer::readWord()
{
    std::string value;

    while(1)
    {
        value += m_currentChar;

        // Read a character, break on EOF.
        if(!readChar(false))
            break;

        if(!canBeInWord(m_currentChar))
            break;
    };


    return Token(Token::TokenType::Word, value, m_file, m_line, m_column);
}

Token Tokenizer::readNumericLiteral()
{
    std::string value;

    do
    {
        value += m_currentChar;

        // Read a character, break on EOF.
        if(!readChar(false))
            break;
    } while(std::isdigit(m_currentChar) || m_currentChar == '.');


    return Token(Token::TokenType::NumericLiteral, value, m_file, m_line, m_column);
}

Token Tokenizer::readStringLiteral()
{
    std::string value;

    // Read the character after the ".
    readChar(true);

    while(1)
    {
        if(m_currentChar == '\"')
        {
            break;
        }
        else
        {
            value += m_currentChar;
            readChar(true);
        }
    }

    // Eat the ".
    readChar(false);

    return Token(Token::TokenType::StringLiteral, value, m_file, m_line, m_column);
}

Token Tokenizer::readSymbol()
{
    Token result = Token::Null;

    /*
     * Reads a character from the stream, checking it and creating
     * a token representing a symbol if possible.
     * Note: all case handlers MUST read an extra character after
     *       its data is processed so that we can read the token
     *       for that character afterwards.
     */
    switch(m_currentChar)
    {

    // Binary arithmetic operators.
    case '+':
    case '-':
    case '*':
    case '/':
    {
        char first = m_currentChar;

        readChar(false); // Eat the operator.

        return Token(Token::TokenType::Symbol, std::string(1, first), m_file, m_line, m_column);

    }
    case '(':
    case ')':
    case '[':
    case ']':
    case ',':
    case '.':
    case ':':
    {
        result = Token(Token::TokenType::Symbol, std::string(1, m_currentChar), m_file, m_line, m_column);

        // Read a character that will be processed next.
        readChar(false);

        return result;
    }
    case '=':
    {
        char first = m_currentChar;

        readChar(false); // Eat the operator.

        return Token(Token::TokenType::Symbol, std::string(1, first), m_file, m_line, m_column);
    }
    case '<':
    {
        // Attempt to read the next character.
        if(readChar(false))
        {
            if(m_currentChar == '>') // The symbol is a not ('<>') operator.
            {
                readChar(false); // Eat the '>'.
                return Token(Token::TokenType::Symbol, "<>", m_file, m_line, m_column);
            }
            else if(m_currentChar == '=') // The symbol is a '<=' operator.
            {
                readChar(false); // Eat the '='.
                return Token(Token::TokenType::Symbol, "<=", m_file, m_line, m_column);
            }
        }

        return Token(Token::TokenType::Symbol, "<", m_file, m_line, m_column); // Return the less than operator.
    }
    case '>':
    {
        if(readChar(false))
        {
            if(m_currentChar == '=') // The token is a '>=' operator.
            {
                readChar(false); // Eat the '='.
                return Token(Token::TokenType::Symbol, ">=", m_file, m_line, m_column);
            }
        }

        return Token(Token::TokenType::Symbol, ">", m_file, m_line, m_column);
    }
    default:
        throw std::runtime_error("unexpected token");
    }
}

bool Tokenizer::readChar(bool throwException)
{
    try
    {
        m_currentChar = m_reader.readChar();

        if(m_currentChar == '\n')
            m_column = 1;
        else
            m_column++;

    } catch(std::runtime_error e) // end of stream.
    {
        m_finished = true;

        if(throwException)
            throw e;
        else
            return false;
    }

    return true;
}
