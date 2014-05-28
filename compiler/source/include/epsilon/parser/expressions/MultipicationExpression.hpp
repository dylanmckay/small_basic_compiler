#pragma once

#include "BinaryOperationExpression.hpp"

#include <stdexcept>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class MultipicationExpression : public BinaryOperationExpression
            {

            public:

                inline MultipicationExpression(Expression* lhs, Expression* rhs)
                {
                    lhs->grab();
                    rhs->grab();

                    m_lhs = lhs;
                    m_rhs = rhs;
                }

                ~MultipicationExpression()
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
                    return Type::Multipication;
                }

            private:

                Expression* m_lhs;
                Expression* m_rhs;
            };
        }
    }
}
