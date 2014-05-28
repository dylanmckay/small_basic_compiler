#pragma once

#include "ITextReader.hpp"

namespace epsilon
{
    namespace io
    {
        // TODO: change to StringTextReader.
        class StringReader : public ITextReader
        {

        public:

            inline StringReader(const std::string& str) : m_str(str), m_pos(0) { }

            char readChar();

            bool isEndOfStream() const;

        private:

            std::string m_str;
            unsigned int m_pos;
        };
    }
}
