#pragma once

#include "Expression.hpp"

#include <sstream>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            /*!
             * TODO: for integers, have specific token type and numericliteralexpression base class for doubles and ints.
             */
            class NumericLiteralExpression : public Expression
            {

            public:

                /*!
                 * Creates a numeric literal expression from the given double precision floating point number.
                 */
                inline NumericLiteralExpression(double dbl) : m_value(dbl) { }

                inline double getValue() const { return m_value; }

                Expression::Type getExpressionType() const
                {
                    return Type::NumericLiteral;
                }

            private:

                double m_value;

            };
        }
    }
}
