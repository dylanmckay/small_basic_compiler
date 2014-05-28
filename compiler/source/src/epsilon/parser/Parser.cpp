#include "../../../include/epsilon/parser/Parser.hpp"

#include <stdexcept>
#include <vector>
#include <sstream>

using namespace epsilon;
using namespace parser;
using namespace statements;
using namespace expressions;

// Sybol tokens.
const static Token TokenLeftParenthesis(Token::TokenType::Symbol, "(");
const static Token TokenRightParenthesis(Token::TokenType::Symbol, ")");
const static Token TokenPlusSign(Token::TokenType::Symbol, "+");
const static Token TokenMinusSign(Token::TokenType::Symbol, "-");
const static Token TokenMulSign(Token::TokenType::Symbol, "*");
const static Token TokenDivSign(Token::TokenType::Symbol, "/");
const static Token TokenComma(Token::TokenType::Symbol, ",");
const static Token TokenEquals(Token::TokenType::Symbol, "=");
const static Token TokenLessThan(Token::TokenType::Symbol, "<");
const static Token TokenGreaterThan(Token::TokenType::Symbol, ">");
const static Token TokenLessThanOrEqualTo(Token::TokenType::Symbol, "<=");
const static Token TokenGreaterThenOrEqualTo(Token::TokenType::Symbol, ">=");
const static Token TokenNotEquals(Token::TokenType::Symbol, "<>");
const static Token TokenColon(Token::TokenType::Symbol, ":");
const static Token TokenLeftSquareBracket(Token::TokenType::Symbol, "[");
const static Token TokenRightSquareBracket(Token::TokenType::Symbol, "]");
const static Token TokenPeriod(Token::TokenType::Symbol, ".");

// Word tokens.
const static Token TokenSub(Token::TokenType::Word, "Sub");
const static Token TokenEndSub(Token::TokenType::Word, "EndSub");
const static Token TokenIf(Token::TokenType::Word, "If");
const static Token TokenElseIf(Token::TokenType::Word, "ElseIf");
const static Token TokenEndIf(Token::TokenType::Word, "EndIf");
const static Token TokenThen(Token::TokenType::Word, "Then");
const static Token TokenElse(Token::TokenType::Word, "Else");
const static Token TokenGoto(Token::TokenType::Word, "Goto");
const static Token TokenWhile(Token::TokenType::Word, "While");
const static Token TokenEndWhile(Token::TokenType::Word, "EndWhile");
const static Token TokenFor(Token::TokenType::Word, "For");
const static Token TokenEndFor(Token::TokenType::Word, "EndFor");
const static Token TokenTo(Token::TokenType::Word, "To");
const static Token TokenStep(Token::TokenType::Word, "Step");
const static Token TokenAnd(Token::TokenType::Word, "And");
const static Token TokenOr(Token::TokenType::Word, "Or");

statements::Statement* Parser::parseNextStatement()
{
    if(!m_started)
    {
        m_started = true;
        readNextToken(false);
    }

    if(isEndOfStream())
        return nullptr;

    // Eat new lines we don't care about.
    while(m_currentToken == Token::NewLine)
        readNextToken(false);

    // Check for new line.
    if(isEndOfStream())
        return nullptr;

    // All statements must start with a word.
    if(m_currentToken.getType() != Token::TokenType::Word)
    {
        onError("expected word");
        return nullptr;
    }

    if(Token::Compare(m_strCmp, m_currentToken, TokenSub)) // Check for a 'Sub' statement.
    {
        return parseSubroutoneStatement();
    }
    else if(Token::Compare(m_strCmp, m_currentToken, TokenIf)) // Check for an 'If' statement.
    {
        return parseIfStatement();
    }
    else if(Token::Compare(m_strCmp, m_currentToken, TokenElseIf)) // Check for an 'ElseIf' statement.
    {
        return parseElseIfStatement();
    }
    else if(Token::Compare(m_strCmp, m_currentToken, TokenWhile)) // Check for a 'While' statement.
    {
        return parseWhileStatement();
    }
    else if(Token::Compare(m_strCmp, m_currentToken, TokenFor)) // Check for a 'For' statement.
    {
        return parseForStatement();
    }
    else if(Token::Compare(m_strCmp, m_currentToken, TokenGoto)) // Check for a 'Goto' statement.
    {
        return parseGotoStatement();
    }

    return parseIdentifierStatement();
}

bool Parser::readNextToken(bool careEOF)
{
    m_currentToken = m_tokenizer.readNextToken();

    if(m_currentToken == Token::Null)
    {
        if(careEOF)
        {
            onError("unexpected end of stream");
            return true;
        }
        else
            return false;
    }
    else
        return true; // Success.
}

void Parser::expect(const Token& token)
{
    Token current = m_currentToken;

    readNextToken(false);

    if(!Token::Compare(m_strCmp, current, token))
    {
        onError("expected '" + token.getValue() + "'");
        return;
    }
}

void Parser::expectEOL()
{
    if(m_currentToken != Token::NewLine)
    {
        onError("expected a new line");
        return;
    }

    readNextToken(false); // Eat the new line.
}

statements::Statement* Parser::parseIdentifierStatement()
{
    std::string first = m_currentToken.getValue();

    // Attempt to read a token.
    if(readNextToken(false))
    {
        if(m_currentToken == TokenColon) // The statement is a label.
        {
            readNextToken(true); // Eat the ':'.

            expectEOL(); // Expect a new line.

            return new LabelStatement(first);
        }


        if(m_currentToken == TokenPeriod)
        {
            readNextToken(true); // Eat the '.'.

            // Parse the name.
            while(1)
            {
                if((m_currentToken.getType() == Token::TokenType::Word))
                    first += "." + m_currentToken.getValue();
                else
                {
                    onError("expected identifier, '.', '=' or '('");
                    return nullptr;
                }

                readNextToken(true); // Eat the last part of the name.

                if(m_currentToken == TokenPeriod)
                {
                    readNextToken(true);
                    continue;
                }
                else if((m_currentToken == TokenLeftParenthesis) || (m_currentToken == TokenEquals))
                    break;
            }
        }

        // The statement is a function call statement.
        if(m_currentToken == TokenLeftParenthesis)
        {
            std::vector<Expression*> args; // The arguments to the function.

            parseFunctionCall(args); // Parse the function call.

            expectEOL(); // Expect an EOL.

            FunctionCallStatement* statement = new FunctionCallStatement(first, args);

            // Drop the argument list.
            for(unsigned int i=0; i<args.size(); i++)
                args[i]->drop();

            return statement;
        }

        Expression* lhs = new VariableExpression(first);

        while(m_currentToken == TokenLeftSquareBracket)
        {
            Expression* array = lhs;

            lhs = parseArrayIndexExpression(array);

            array->drop();
        }

        if(m_currentToken == TokenEquals) // The statement is in the form 'x = y'.
        {
            readNextToken(true); // Eat the '='.

            Expression* rhs = parseExpression();

            expectEOL();

            AssignmentStatement* statement = new AssignmentStatement(lhs, rhs);

            rhs->drop();

            return statement;
        }
    }


    onError("expected '(', '=' or ':'");

    return nullptr;
}

statements::SubroutineStatement* Parser::parseSubroutoneStatement()
{
    std::string name;
    std::vector<Statement*> statements; // Holds the statements in the sub.

    readNextToken(true); // Eat the 'Sub'.

    if(m_currentToken.getType() != Token::TokenType::Word)
    {
        onError("expected subroutine name");
        return nullptr;
    }

    name = m_currentToken.getValue();

    readNextToken(true); // Eat the sub name.

    expectEOL(); // Expect a new line.

    while(1)
    {
        eatWhitespace();

        Statement* statement;

        // Check for an EndSub.
        if(Token::Compare(m_strCmp, m_currentToken, TokenEndSub))
            break;

        // Read a statement inside the subroutine.
        statement = parseNextStatement();

        // Check for unexpected EOF.
        if(!statement)
        {
            onError("expected statement or '" + TokenEndSub.getValue() + "'");
            return nullptr;
        }

        // Add the statement to the list.
        statements.push_back(statement);

    }

    readNextToken(false); // Eat the 'EndSub'.

    expectEOL(); // Expect a new line.

    // Create the statement.
    SubroutineStatement* statement = new SubroutineStatement(name, statements);

    // Drop the statements.
    for(unsigned int i=0; i<statements.size(); i++)
        statements[i]->drop();

    return statement;
}

statements::IfStatement* Parser::parseIfStatement()
{
    Expression*             expression;
    std::vector<Statement*> trueStatements;
    std::vector<Statement*> elseStatements;

    readNextToken(true); // Eat the 'If'.

    expression = parseExpression(); // Parse the expression.

    // Expect 'Then'.
    expect(TokenThen);

    // Read the on true statements.
    while(1)
    {
        eatWhitespace();

        // Check if the if block is finished.
        if(Token::Compare(m_strCmp, m_currentToken, TokenEndIf) || Token::Compare(m_strCmp, m_currentToken, TokenElseIf))
            goto onEndifOrElseif;
        else if(Token::Compare(m_strCmp, m_currentToken, TokenElse)) // There's a false block.
        {
            readNextToken(true); // Eat the 'Else.
            break; // Process it after the loop.
        }

        // Read a statement.
        Statement* statement = parseNextStatement();

        // Add it to the on true statement list.
        trueStatements.push_back(statement);

    }


    // The 'Else has already been eaten, if it even existed.

    // Read the on false statements (if any).
    while(1)
    {
        eatWhitespace();

        if(Token::Compare(m_strCmp, m_currentToken, TokenEndIf) || Token::Compare(m_strCmp, m_currentToken, TokenElseIf))
            goto onEndifOrElseif;

        // Parse a statement.
        Statement* statement = parseNextStatement();

        // Add it to the else statement block.
        elseStatements.push_back(statement);
    }


    onEndifOrElseif:

    // If the current token is an 'EndIf', eat it, but if it is an 'ElseIf', leave it.
    if(Token::Compare(m_strCmp, m_currentToken, TokenEndIf))
    {
        readNextToken(true); // Eat the 'EndIf'.
        expectEOL(); // Expect an EOL.
    }

    IfStatement* statement = new IfStatement(expression, trueStatements, elseStatements);

    // Drop the true statement block.
    for(unsigned int i=0; i<trueStatements.size(); i++)
        trueStatements[i]->drop();

    // Drop the else statement block.
    for(unsigned int i=0; i<elseStatements.size(); i++)
        elseStatements[i]->drop();

    return statement;
}

statements::ElseIfStatement* Parser::parseElseIfStatement()
{
    Expression*             expression;
    std::vector<Statement*> trueStatements;
    std::vector<Statement*> elseStatements;

    readNextToken(true); // Eat the 'ElseIf'.

    expression = parseExpression(); // Parse the expression.

    // Expect 'Then'.
    expect(TokenThen);

    // Read the on true statements.
    while(1)
    {
        eatWhitespace();

        // Check if the if block is finished.
        if(Token::Compare(m_strCmp, m_currentToken, TokenEndIf) || Token::Compare(m_strCmp, m_currentToken, TokenElseIf))
            goto onEndifOrElseif;
        else if(Token::Compare(m_strCmp, m_currentToken, TokenElse)) // There's a false block.
        {
            readNextToken(true); // Eat the 'Else.
            break; // Process it after the loop.
        }

        // Read a statement.
        Statement* statement = parseNextStatement();

        // Add it to the on true statement list.
        trueStatements.push_back(statement);

    }


    // The 'Else has already been eaten, if it even existed.

    // Read the on false statements (if any).
    while(1)
    {
        eatWhitespace();

        if(Token::Compare(m_strCmp, m_currentToken, TokenEndIf) || Token::Compare(m_strCmp, m_currentToken, TokenElseIf))
            goto onEndifOrElseif;

        // Parse a statement.
        Statement* statement = parseNextStatement();

        // Add it to the else statement block.
        elseStatements.push_back(statement);
    }


    onEndifOrElseif:

    // If the current token is an 'EndIf', eat it, but if it is an 'ElseIf', leave it.
    if(Token::Compare(m_strCmp, m_currentToken, TokenEndIf))
    {
        readNextToken(true); // Eat the 'EndIf'.
        expectEOL(); // Expect an EOL.
    }

    ElseIfStatement* statement = new ElseIfStatement(expression, trueStatements, elseStatements);

    // Drop the true statement block.
    for(unsigned int i=0; i<trueStatements.size(); i++)
        trueStatements[i]->drop();

    // Drop the else statement block.
    for(unsigned int i=0; i<elseStatements.size(); i++)
        elseStatements[i]->drop();

    return statement;
}

statements::WhileStatement* Parser::parseWhileStatement()
{
    Expression*               expression;
    std::vector<Statement*>   statements;

    readNextToken(true); // Eat 'While'.

    expression = parseExpression(); // Parse the expression.

    expectEOL(); // Expect an end of line.

    while(1)
    {
        eatWhitespace();

        if(Token::Compare(m_strCmp, m_currentToken, TokenEndWhile))
            break;

        Statement* statement = parseNextStatement(); // Parse a statement.

        statements.push_back(statement); // Add it to the statement list.
    }

    readNextToken(true); // Eat the 'EndWhile'.

    expectEOL(); // Expect a new line.

    // Create the statement.
    WhileStatement* statement = new WhileStatement(expression, statements);

    // Drop the statements list.
    for(unsigned int i=0; i<statements.size(); i++)
        statements[i]->drop();

    return statement;
}

statements::ForStatement* Parser::parseForStatement()
{
    std::string varName;                // The variable that is to be incremented.
    Expression* initVal;                // The initial value of the variable.
    Expression* top;                    // The value which will be counted up to.
    Expression* step = nullptr;         // The for loop step, defaults to null meaning '1' (default step).s
    std::vector<Statement*> statements; // The statements in the loop.

    readNextToken(true); // Eat the 'For'.

    // Verify that the variable name is a word.
    if(m_currentToken.getType() != Token::TokenType::Word)
    {
        onError("expected variable name");
        return nullptr;
    }

    varName = m_currentToken.getValue(); // Save the variable name.

    readNextToken(true); // Eat the variable name.

    expect(TokenEquals); // Expect an equals sign.

    initVal = parseExpression(); // Parse the initial value.

    expect(TokenTo); // Expect 'To'.

    top = parseExpression(); // Parse the top value.

    // Check if there's a step.
    if(m_currentToken == TokenStep)
    {
        readNextToken(true); // Eat 'Step'.

        step = parseExpression(); // Parse the step.
    }

    expectEOL(); // Expect an EOL.

    while(1)
    {
        eatWhitespace();

        // Check if an end for has been reached.
        if(Token::Compare(m_strCmp, m_currentToken, TokenEndFor))
            break;

        // Parse a statement.
        Statement* statement = parseNextStatement();

        statements.push_back(statement);
    }

    readNextToken(true); // Eat 'EndFor'.

    expectEOL(); // Expect new line.

    ForStatement* statement = new ForStatement(varName, initVal, top, statements, step);

    for(unsigned int i=0; i<statements.size(); i++)
        statements[i]->drop();

    return statement;
}

statements::GotoStatement* Parser::parseGotoStatement()
{
    readNextToken(true); // Eat the 'Goto'.

    // Verify that the goto target is an identifier.
    if(m_currentToken.getType() != Token::TokenType::Word)
    {
        onError("unexpected token, expected label name");
        return nullptr;
    }

    // Read the label name.
    std::string labelName = m_currentToken.getValue();

    readNextToken(true); // Eat the label name.

    expectEOL(); // Expect an EOL.

    return new GotoStatement(labelName);
}

int Parser::getOperatorPrecedence(const Token& op)
{

    if(op == TokenEquals)
        return 1000;
    else if(op == TokenNotEquals)
        return 1000;
    else if(op == TokenLessThan)
        return 30;
    else if(op == TokenGreaterThan)
        return 30;
    else if(op == TokenPlusSign)
        return 50;
    else if(op == TokenMinusSign)
        return 50;
    else if(op == TokenMulSign)
        return 70;
    else if(op == TokenDivSign)
        return 70;
    else if(Token::Compare(m_strCmp, op, TokenAnd))
        return 15;
    else if(Token::Compare(m_strCmp, op, TokenOr))
        return 15;
    else if(op == TokenLessThanOrEqualTo)
        return 50;
    else if(op == TokenGreaterThenOrEqualTo)
        return 50;
    else
        return -1; // The token has no precedence.
}

expressions::Expression* Parser::parseExpression()
{
    Expression* lhs = parsePrimaryExpression();

    if(!lhs)
        return NULL;

    Expression* expr =  parseBinaryOperationRHS(0, lhs);

    if(lhs != expr)
        lhs->drop();

    return expr;
}

expressions::Expression* Parser::parsePrimaryExpression()
{
    if(isEndOfStream())
        return NULL;

    if(m_currentToken == TokenMinusSign)
    {
        readNextToken(true); // Eat the '-'.

        Expression* expr = parseExpression();

        NegativeExpression* result = new NegativeExpression(expr);

        expr->drop();

        return result;
    }
    if(m_currentToken.getType() == Token::TokenType::NumericLiteral) // Check for a numeric literal.
    {
        return parseNumericLiteralExpression();
    }
    else if(m_currentToken.getType() == Token::TokenType::StringLiteral) // Check for a string literal.
    {
        return parseStringLiteralExpression();
    }
    else if(m_currentToken.getType() == Token::TokenType::Word) // Check for a word.
    {
        return parseWordExpression();
    }
    else if(Token::Compare(m_strCmp, m_currentToken, TokenLeftParenthesis)) // Check for a parentheses expression.
    {
        return parseParenthesesExpression();
    }
    else // Unexpected token.
    {
        onError("unexpected token");
        return nullptr;
    }

}

expressions::Expression* Parser::parseBinaryOperationRHS(int expressionPrecedence, expressions::Expression* lhs)
{
    Expression* origLHS = lhs;

    while(1)
    {
        int tokPrec = getOperatorPrecedence(m_currentToken);

        if(tokPrec < expressionPrecedence)
            return lhs;

        Token op = m_currentToken;
        readNextToken(true);

        Expression* rhs = parsePrimaryExpression();

        if(!rhs) return nullptr;

        int nextPrec = getOperatorPrecedence(m_currentToken);

        if(tokPrec < nextPrec)
        {
            Expression* oldRHS = rhs;

            rhs = parseBinaryOperationRHS(tokPrec+1, rhs);

            oldRHS->drop();

            if(!rhs) return nullptr;
        }

        Expression* oldLHS = lhs;

        // Check the operator.
        if(op == TokenPlusSign)
        {
            lhs = new AdditionExpression(lhs, rhs); // Operator is addition.
        }
        else if(op == TokenMinusSign)
        {
            lhs = new SubtractionExpression(lhs, rhs); // Operator is subtraction.
        }
        else if(op == TokenMulSign)
        {
            lhs = new MultipicationExpression(lhs, rhs); // Operator is multipication.
        }
        else if(op == TokenDivSign)
        {
            lhs = new DivisionExpression(lhs, rhs); // Operator is division.
        }
        else if(op == TokenEquals)
        {
            lhs = new EqualExpression(lhs, rhs);    // Operator is equal comparison.
        }
        else if(op == TokenLessThan)
        {
            lhs = new LessThanExpression(lhs, rhs);  // Operator is less than.
        }
        else if(op == TokenGreaterThan)
        {
            lhs = new GreaterThanExpression(lhs, rhs); // Operator is greater than.
        }
        else if(op == TokenLessThanOrEqualTo)
        {
            lhs = new LessThanOrEqualToExpression(lhs, rhs); // Operator is less than or equal to.
        }
        else if(op == TokenGreaterThenOrEqualTo)
        {
            lhs = new GreaterThanOrEqualToExpression(lhs, rhs); // Operator is greater than or equal to.
        }
        else if(op == TokenNotEquals)
        {
            lhs = new NotEqualExpression(lhs, rhs);   // Operator is not equal.
        }
        else if(Token::Compare(m_strCmp, op, TokenAnd))
        {
            lhs = new AndExpression(lhs, rhs);
        }
        else if(Token::Compare(m_strCmp, op, TokenOr))
        {
            lhs = new OrExpression(lhs, rhs);
        }
        else
        {
            onError("undefined operator '" + op.getValue() + "'");
            return nullptr;
        }

        if(oldLHS !=origLHS)
            oldLHS->drop(); // TODO:

        rhs->drop();

    }
}

expressions::NumericLiteralExpression* Parser::parseNumericLiteralExpression()
{
    bool negative = false;

    if(m_currentToken == TokenMinusSign)
    {
        negative = true;
        readNextToken(true);
    }

    double value = std::atof(m_currentToken.getValue().c_str());

    if(negative)
        value *= -1; // Make it negative.

    readNextToken(false);

    return new NumericLiteralExpression(value);
}

expressions::StringLiteralExpression* Parser::parseStringLiteralExpression()
{
    std::string value = m_currentToken.getValue();

    readNextToken(false); // Eat the string literal.

    return new StringLiteralExpression(value);
}

expressions::Expression* Parser::parseWordExpression()
{
    std::string first = m_currentToken.getValue();

    readNextToken(false); // Eat the identifier.

    if(m_currentToken == TokenPeriod)
    {
        readNextToken(true); // Eat the '.'.

        // Parse the name.
        while(1)
        {
            if((m_currentToken.getType() == Token::TokenType::Word))
                first += "." + m_currentToken.getValue();
            else
            {
                break;
            }

            readNextToken(true); // Eat the last part of the name.

            if(m_currentToken == TokenPeriod)
            {
                readNextToken(true);
                continue;
            }
            else if((m_currentToken == TokenLeftParenthesis))
                break;
        }
    }

    // It has a '(', it is a function call expression.
    if(m_currentToken == TokenLeftParenthesis)
    {
        std::vector<Expression*> args;

        parseFunctionCall(args); // Parse the argument list.

        // Create the function call expression.
        Expression* expr = new FunctionCallExpression(first, args);

        // Check if we're indexing an item from the return value.
        if(m_currentToken == TokenLeftSquareBracket)
        {
            ArrayIndexExpression* indexExpr = parseArrayIndexExpression(expr);

            expr->drop(); // Drop the old expression.

            expr = indexExpr; // The expression is now the new index expression.
        }

        // Drop the argument list.
        for(unsigned int i=0; i<args.size(); i++)
            args[i]->drop();

        return expr;
    }

    Expression* expr = new VariableExpression(first);

    while(m_currentToken == TokenLeftSquareBracket)
    {
        Expression* array = expr;

        expr = parseArrayIndexExpression(array);

        array->drop();
    }

    return expr;
}

expressions::Expression* Parser::parseParenthesesExpression()
{
    readNextToken(true); // Eat '('.

    Expression* expr = parseExpression();

    expect(TokenRightParenthesis); // Expect ')'.

    return expr;
}

expressions::ArrayIndexExpression* Parser::parseArrayIndexExpression(expressions::Expression* array)
{
    array->grab();
    readNextToken(true); // Eat the '['.

    Expression* index = parseExpression(); // Parse the index.

    expect(TokenRightSquareBracket); // Expect a ']'.

    // Create the expression.
    ArrayIndexExpression* expr = new ArrayIndexExpression(array, index);

    // Drop temps.
    array->drop(); // We made a copy, drop our copy.
    index->drop();

    return expr;
}

void Parser::parseFunctionCall(std::vector<expressions::Expression*>& args)
{
    readNextToken(true); // Eat the ')'.

    while(1)
    {
        // Check if the argument list has been closed.
        if(m_currentToken == TokenRightParenthesis)
            break;

        // Read the argument.
        Expression* expr = parseExpression();

        // Add the argument to the list.
        args.push_back(expr);

        // Check if a comma was reached.
        if(m_currentToken == TokenComma)
        {
            readNextToken(true); // Eat comma.
            continue;
        }
        else if(m_currentToken != TokenRightParenthesis)
        {
            onError("expected ')'");
            return;
        }
    }

    readNextToken(false); // Eat the ')'.
}

void Parser::onError(const std::string& message)
{
    unsigned int line = m_currentToken.getLine();
    unsigned int col  = m_currentToken.getColumn();

    std::ostringstream stream;

    stream << "[File]:" << line << ":" << col << " " << message;

    throw std::runtime_error(stream.str());
}

void Parser::eatWhitespace()
{
    while(m_currentToken.getType() == Token::TokenType::NewLine)
        readNextToken(true);
}
