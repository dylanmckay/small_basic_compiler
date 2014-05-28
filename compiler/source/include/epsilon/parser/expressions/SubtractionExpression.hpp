#pragma once

#include "BinaryOperationExpression.hpp"

#include <stdexcept>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class SubtractionExpression : public BinaryOperationExpression
            {

            public:

                inline SubtractionExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~SubtractionExpression()
                {
                    m_lhs->drop();
                    m_rhs->drop();
                }

                const Expression* getLHS() const
                {
                    return m_lhs;
                }

                const Expression* getRHS() const
                {
                    return m_rhs;
                }

                Expression::Type getExpressionType() const
                {
                    return Type::Subtraction;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
