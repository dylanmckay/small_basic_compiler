#pragma once

#include "IStringComparator.hpp"

#include <cctype>

namespace epsilon
{
    class SBStringComparator : public IStringComparator
    {

    public:

        bool compare(const std::string& str1, const std::string& str2) const
        {
            std::string cmpStr1, cmpStr2;


            unsigned int str1Len = str1.length();
            unsigned int str2Len = str2.length();

            for(unsigned int i=0; i<str1Len; i++)
                cmpStr1 += ::tolower(str1[i]);

            for(unsigned int i=0; i<str2Len; i++)
                cmpStr2 += ::tolower(str2[i]);

            return cmpStr1 == cmpStr2;
        }
    };
}
