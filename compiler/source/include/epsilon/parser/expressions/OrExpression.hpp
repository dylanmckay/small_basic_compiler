#pragma once

#include "BinaryOperationExpression.hpp"

#include <string>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class OrExpression : public Expression
            {

            public:

                inline OrExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~OrExpression()
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
                    return Type::Or;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
