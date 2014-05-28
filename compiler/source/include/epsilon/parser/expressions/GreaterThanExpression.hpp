#pragma once

#include "BinaryOperationExpression.hpp"

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class GreaterThanExpression : public BinaryOperationExpression
            {

            public:

                inline GreaterThanExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~GreaterThanExpression()
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
                    return Type::GreaterThan;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
