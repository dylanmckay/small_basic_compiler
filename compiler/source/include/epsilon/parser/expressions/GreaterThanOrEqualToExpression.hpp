#pragma once

#include "BinaryOperationExpression.hpp"

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class GreaterThanOrEqualToExpression : public BinaryOperationExpression
            {

            public:

                inline GreaterThanOrEqualToExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~GreaterThanOrEqualToExpression()
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
                    return Type::GreaterThanOrEqualTo;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
