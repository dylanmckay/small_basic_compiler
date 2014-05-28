#include "../../../include/epsilon/parser/Token.hpp"

using namespace epsilon;
using namespace parser;

static Token NullToken(Token::TokenType::None, "");
Token& Token::Null = NullToken;

static Token NewLineToken(Token::TokenType::NewLine, "");
Token& Token::NewLine = NewLineToken;

Token Token::GetNewLine(const std::string& file, unsigned int line, unsigned int column)
{
    return Token(Token::TokenType::NewLine, "", file, line, column);
}
