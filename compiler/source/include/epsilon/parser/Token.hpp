#pragma once

#include "../IStringComparator.hpp"

#include <string>


namespace epsilon
{
    namespace parser
    {
        class Token
        {

        public:

            enum class TokenType
            {
                Word,            // TODO: better wording.
                StringLiteral,   // A string encapsulated in double quotes.
                NumericLiteral,  // A number (either integer or floating point).
                Symbol,          // A symbol.
                NewLine,         // A new line.
                None             // Internal.
            };

            /*!
             * Represents a null token.
             */
            static Token& Null;

            /*!
             * Represents a new line token.
             */
            static Token& NewLine;

            static Token GetNewLine(const std::string& file, unsigned int line, unsigned int column);

            /*!
             * Compares two tokens with a string comparator.
             * \param comparator The string comparator.
             * \param tok1 The first token to compare.
             * \param tok2 The second token to compare.
             */
            inline static bool Compare(const IStringComparator& comparator, const Token& tok1, const Token& tok2)
            {
                return (tok1.m_type == tok2.m_type) && (comparator.compare(tok1.m_value, tok2.m_value));
            }






            inline Token(TokenType type, const std::string& value) : m_type(type), m_value(value) { }

            inline Token(TokenType type,
                         const std::string& value,
                         const std::string& file,
                         unsigned int line,
                         unsigned int column) : m_type(type),
                                                m_value(value),
                                                m_file(file),
                                                m_line(line),
                                                m_column(column) { }

            inline TokenType getType() const { return m_type; }

            inline const std::string& getValue() const { return m_value; }

            inline const std::string& getFile() const { return m_file; }

            inline unsigned int getLine() const { return m_line; }

            inline unsigned int getColumn() const { return m_column; }


            /*!
             * Performs a case-sensitive comparison of two tokens.
             * \note If you need case-insensitivity, use Token::Compare.
             */
            inline bool operator==(const Token& tok) const
            {
                return (this->m_type == tok.m_type) && (this->m_value == tok.m_value);
            }

            /*!
             * Checks if two tokens are not equal.
             * \note If you need case-insensitivity, use Token::Compare.
             */
            inline bool operator!=(const Token& tok) const
            {
                return !this->operator==(tok);
            }

        private:

            TokenType   m_type;
            std::string m_value;

            std::string m_file;
            unsigned int m_line;
            unsigned int m_column;

        };
    }
}
