#pragma once

#include "Expression.hpp"

#include <string>
#include <sstream>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            /*!
             * An abstract class for binary expressions.
             */
            class BinaryOperationExpression : public Expression
            {


            public:

                virtual inline ~BinaryOperationExpression() { }

                virtual const Expression* getLHS() const = 0;

                virtual const Expression* getRHS() const = 0;


                inline bool isNumeric(const std::string& str) const
                {
                    double d;

                    std::istringstream num(str);

                    num >> d;

                    return (!num.fail() && num.eof());
                }

                /*!
                 * Checks if the given strings are numeric.
                 */
                inline bool areNumeric(const std::string& str1, const std::string& str2) const
                {
                    return isNumeric(str1) && isNumeric(str2);
                }
            };
        }
    }
}
