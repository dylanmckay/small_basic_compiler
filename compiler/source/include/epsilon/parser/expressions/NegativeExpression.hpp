#pragma once

#include "Expression.hpp"

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class NegativeExpression : public Expression
            {

            public:

                inline NegativeExpression(Expression* expr)
                {
                    expr->grab();

                    m_expr = expr;
                }

                ~NegativeExpression()
                {
                    m_expr->drop();
                }

                inline const Expression* getTarget() const
                {
                    return m_expr;
                }

                Expression::Type getExpressionType() const
                {
                    return Type::Negative;
                }

            private:

                Expression* m_expr;
            };
        }
    }
}
