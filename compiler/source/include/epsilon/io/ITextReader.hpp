#pragma once

#include <string>

namespace epsilon
{
    namespace io
    {
        class ITextReader
        {

        public:

            virtual inline ~ITextReader() { }

            virtual char readChar() = 0;

            virtual std::string readLine()
            {
                std::string result;

                while(!isEndOfStream())
                {
                    char c = readChar();

                    if(c == '\n')
                        return result;
                    else if(c == '\r')
                    {
                        continue;
                    }
                    else
                        result += c;
                }

                return result;
            }

            virtual std::string readToEnd()
            {
                std::string result;

                while(!isEndOfStream())
                    result += readLine();

                return result;
            }

            virtual bool isEndOfStream() const = 0;

        };
    }
}
