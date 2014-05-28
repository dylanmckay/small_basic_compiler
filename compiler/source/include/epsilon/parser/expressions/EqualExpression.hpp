#pragma once

#include "BinaryOperationExpression.hpp"

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class EqualExpression : public BinaryOperationExpression
            {

            public:

                inline EqualExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~EqualExpression()
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
                    return Type::Equal;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
