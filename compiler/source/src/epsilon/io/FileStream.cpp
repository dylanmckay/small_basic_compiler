#include "../../../include/epsilon/io/FileStream.hpp"

#include <stdexcept>
#include <cstring>

using namespace epsilon;
using namespace io;

FileStream::FileStream(const std::string& path, FileMode mode)
{
    const char* libcMode;

    if(mode == FileMode::Create)
        libcMode = "w+";
    else
        libcMode = "r+";

    m_file = std::fopen(path.c_str(), libcMode);

    if(!m_file)
        throw std::runtime_error(std::strerror(errno));
}

FileStream::~FileStream()
{
    fclose(m_file);
}

void FileStream::close()
{
    // Attempt to close.
    if(std::fclose(m_file) == EOF) // It failed.
    {
        m_file = nullptr;
        throw std::runtime_error(std::strerror(errno));
    }

    m_file = nullptr;

}

void FileStream::seek(unsigned int pos)
{
    if(std::fseek(m_file, pos, SEEK_SET) != 0)
        throw std::runtime_error(std::strerror(errno));
}

void FileStream::write(const void* data, unsigned int size)
{
    if(std::fwrite(data, size, 1, m_file) != size)
        throw std::runtime_error(std::strerror(errno)); // THIS FUNCTION MAY NOT SET errno.
}

unsigned int FileStream::read(void* buffer, unsigned int size)
{
    unsigned int count = std::fread(buffer, size, 1, m_file);

    if(count != size)
    {
        if(feof(m_file) != 0)
            return count;
        else
            throw std::runtime_error(std::strerror(ferror(m_file)));
    }

    return count;
}

bool FileStream::isEOF() const
{
    return feof(m_file) != 0;
}

bool FileStream::supportsSeeking() const
{
    return true;
}
