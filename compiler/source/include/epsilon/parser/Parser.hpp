#pragma once

#include "Tokenizer.hpp"
#include "../SBStringComparator.hpp"
#include "statements/statements.hpp"
#include "expressions/expressions.hpp"

namespace epsilon
{
    namespace parser
    {
        class Parser
        {

        public:

            inline Parser(io::ITextReader& reader) : m_tokenizer(reader),
                                                     m_currentToken(Token::Null),
                                                     m_started(false) { }


            //inline void tempBegin() { readNextToken(false); m_started = true; }

            statements::Statement* parseNextStatement();

            /*!
             * Checks if the end of the stream has been reached.
             */
            inline bool isEndOfStream() const { return m_currentToken == Token::Null; }

        private:

            Tokenizer m_tokenizer;

            Token m_currentToken;

            SBStringComparator m_strCmp;

            bool m_started; // TODO: redundant?

            /*!
             * Compares two tokens.
             * \todo Not sure if this is used.
             */
            inline bool compareToken(const Token& tok1, const Token& tok2) { return Token::Compare(m_strCmp, tok1, tok2); }

            /*!
             * Reads the next token.
             * \param careEOF 'true' if an error shall be generated if the end of stream is reached, 'false' otherwise.
             * \return 'true' if the end of stream was reached, 'false' otherwise.
             */
            bool readNextToken(bool careEOF);

            /*!
             * Checks if the current token is the given token and reads the next character. If the token is not as expected an error is generated.
             * \param token The expected token.
             */
            void expect(const Token& token);

            /*!
             * Same as expect(token) but checks for a new line.
             */
            void expectEOL();

            /*!
             * Parses a statement that starts with an identifier.
             */
            statements::Statement* parseIdentifierStatement();

            statements::SubroutineStatement* parseSubroutoneStatement();

            statements::IfStatement* parseIfStatement();

            statements::ElseIfStatement* parseElseIfStatement();

            statements::WhileStatement* parseWhileStatement();

            statements::ForStatement* parseForStatement();

            statements::GotoStatement* parseGotoStatement();

            /*!
             * Gets the operator precedence specifier for the given token.
             * \param op The operator.
             * \return The precedence of the operator or -1 if the token has no precedence.
             */
            int getOperatorPrecedence(const Token& op);

            expressions::Expression* parseExpression();

            expressions::Expression* parsePrimaryExpression();

            expressions::Expression* parseBinaryOperationRHS(int expressionPrecedence, expressions::Expression* lhs);

            expressions::NumericLiteralExpression* parseNumericLiteralExpression();

            expressions::StringLiteralExpression* parseStringLiteralExpression();

            expressions::Expression* parseWordExpression();

            expressions::Expression* parseParenthesesExpression();

            expressions::ArrayIndexExpression* parseArrayIndexExpression(expressions::Expression* array);

            /*!
             * Parses the arguments to a function call. The current token when calling this must be the first '(' of the call.
             * \param args A vector which will contain the arguments to the function. The contents of this list must be deleted when done.
             * \note This function will eat everything up to and including the last ')' of the call.
             */
            void parseFunctionCall(std::vector<expressions::Expression*>& args);

            void onError(const std::string& message);

            void eatWhitespace();
        };
    }
}
