#pragma once

#include <string>

namespace epsilon
{
    class IStringComparator
    {

    public:

        virtual inline ~IStringComparator() { }

        virtual bool compare(const std::string& str1, const std::string& str2) const = 0;
    };
}
