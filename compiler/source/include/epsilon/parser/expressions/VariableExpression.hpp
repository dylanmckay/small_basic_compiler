#pragma once

#include "Expression.hpp"

#include <string>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class VariableExpression : public Expression
            {


            public:

                inline VariableExpression(const std::string& name) : m_name(name) { }

                inline const std::string& getVariableName() const { return m_name; }

                Expression::Type getExpressionType() const
                {
                    return Type::Variable;
                }

            private:

                std::string m_name;
            };
        }
    }
}
