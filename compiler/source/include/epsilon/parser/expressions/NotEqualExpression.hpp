#pragma once

#include "BinaryOperationExpression.hpp"

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class NotEqualExpression : public BinaryOperationExpression
            {

            public:

                inline NotEqualExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~NotEqualExpression()
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
                    return Type::NotEqual;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
