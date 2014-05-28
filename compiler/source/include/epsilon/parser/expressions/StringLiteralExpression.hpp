#pragma once

#include "Expression.hpp"

#include <string>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class StringLiteralExpression : public Expression
            {

            public:

                inline StringLiteralExpression(const std::string& str) : m_str(str) { }

                inline const std::string& getValue() const { return m_str; }

                Expression::Type getExpressionType() const
                {
                    return Type::StringLiteral;
                }

            private:

                std::string m_str;
            };
        }
    }
}
