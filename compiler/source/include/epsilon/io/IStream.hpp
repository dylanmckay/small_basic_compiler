#pragma once

namespace epsilon
{
    namespace io
    {
        class IStream
        {

        public:

            virtual inline ~IStream() { }

            virtual void write(const void* data, unsigned int size) = 0;

            virtual unsigned int read(void* buffer, unsigned int size) = 0;

            virtual bool isEOF() const = 0;

            virtual bool supportsSeeking() const = 0;
        };
    }
}
