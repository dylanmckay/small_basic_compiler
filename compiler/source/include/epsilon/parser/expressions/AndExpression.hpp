#pragma once

#include "BinaryOperationExpression.hpp"

#include <string>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class AndExpression : public Expression
            {

            public:

                inline AndExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~AndExpression()
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
                    return Type::And;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
