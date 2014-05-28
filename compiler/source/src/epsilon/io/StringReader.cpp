#include "../../../include/epsilon/io/StringReader.hpp"

#include <stdexcept>

using namespace epsilon;
using namespace io;

char StringReader::readChar()
{
    if(m_pos >= m_str.length())
        throw std::runtime_error("end of stream");

    return m_str[m_pos++];
}

bool StringReader::isEndOfStream() const
{
    return m_pos >= m_str.length();
}
