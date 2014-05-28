#pragma once

#include "BinaryOperationExpression.hpp"

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class LessThanOrEqualToExpression : public BinaryOperationExpression
            {

            public:

                inline LessThanOrEqualToExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~LessThanOrEqualToExpression()
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
                    return Type::LessThanOrEqualTo;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
