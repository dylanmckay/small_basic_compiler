#include "../../../include/epsilon/io/StreamTextReader.hpp"

#include <stdexcept>

using namespace epsilon;
using namespace io;

StreamTextReader::~StreamTextReader()
{
    if(m_autoDel)
        delete m_stream;
}

char StreamTextReader::readChar()
{
    char buf;

    if(m_stream->read(&buf, 1) != 1)
        throw std::runtime_error("end of stream");

    return buf;
}

bool StreamTextReader::isEndOfStream() const
{
    if(m_stream->supportsSeeking())
        return m_stream->isEOF();
    else // The stream supports seeking.
        return true;
}
