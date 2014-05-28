#pragma once

#include "IStream.hpp"

#include <cstdio>
#include <string>

namespace epsilon
{
    namespace io
    {
        class FileStream : public IStream
        {

        public:

            enum class FileMode
            {
                Create,       // Creates a new file or replaces an existing file. Seeks to the start.
                Open          // Opens an existing file for reading and writing. Seeks to the start. (r+)
            };


            FileStream(const std::string& path, FileMode mode);

            ~FileStream();

            void close();


            void seek(unsigned int pos);

            void write(const void* data, unsigned int size);

            unsigned int read(void* buffer, unsigned int size);

            bool isEOF() const;

            bool supportsSeeking() const;


        private:

            std::FILE* m_file;
        };
    }
}
