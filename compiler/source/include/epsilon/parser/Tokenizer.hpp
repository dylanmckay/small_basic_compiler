#pragma once

#include "Token.hpp"
#include "../io/ITextReader.hpp"
namespace epsilon
{
    namespace parser
    {
        class Tokenizer
        {

        public:

            inline Tokenizer(io::ITextReader& reader) : m_reader(reader),
                                                        m_started(false),
                                                        m_finished(false),
                                                        m_firstEOF(false),
                                                        m_file("file"),
                                                        m_line(1),
                                                        m_column(1) { }

            /*!
             * Reads the next token from the stream.
             * \return The next token, or Token::Null if the end of stream is reached.
             */
            Token readNextToken();

            bool isEndOfStream() const { return m_finished; }

        private:

            // The text reader which contains our tokens.
            io::ITextReader& m_reader;

            // The current character to be read.
            char m_currentChar;

            // Whether reading has been started from the stream.
            bool m_started; // make static.

            bool m_finished;

            bool m_firstEOF;

            std::string m_file;

            unsigned int m_line;

            unsigned int m_column;

            /*!
             * Checks if the current token is whitespace and if so, eats it and all whitespace characters thereafter.
             * \return 'true' if an EOF or EOF marker was passed or reached.
             */
            bool eatWhitespace();

            Token readWord();

            Token readNumericLiteral();

            Token readStringLiteral();

            Token readSymbol();

            bool readChar(bool throwException);

        };
    }
}
