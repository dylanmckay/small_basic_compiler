#pragma once

#include "ITextReader.hpp"

#include "IStream.hpp"

namespace epsilon
{
    namespace io
    {
        class StreamTextReader : public ITextReader
        {

        public:

            inline StreamTextReader(IStream* stream, bool autoDel = false) : m_stream(stream), m_autoDel(autoDel) { }

            ~StreamTextReader();

            char readChar();

            bool isEndOfStream() const;

        private:

            IStream* m_stream;

            bool m_autoDel;
        };
    }
}
