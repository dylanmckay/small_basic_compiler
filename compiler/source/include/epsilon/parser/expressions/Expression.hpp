#pragma once

#include "../../IReferenceCounted.hpp"

#include <stdexcept>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class Expression : public IReferenceCounted
            {

            public:

                enum class Type
                {
                    Addition,
                    And,
                    ArrayIndex,
                    Division,
                    Equal,
                    FunctionCall,
                    GreaterThan,
                    GreaterThanOrEqualTo,
                    LessThan,
                    LessThanOrEqualTo,
                    Multipication,
                    Negative,
                    NotEqual,
                    NumericLiteral,
                    Or,
                    StringLiteral,
                    Subtraction,
                    Variable
                };

                virtual inline ~Expression() { }

                virtual Expression::Type getExpressionType() const = 0;


            };
        }
    }
}
