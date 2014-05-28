#pragma once

#include "Expression.hpp"

#include <string>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class ArrayIndexExpression : public Expression
            {

            public:

                inline ArrayIndexExpression(Expression* array, Expression* index)
                {
                    array->grab();
                    index->grab();

                    m_array = array;
                    m_index = index;
                }

                ~ArrayIndexExpression()
                {
                    m_array->drop();
                    m_index->drop();
                }

                inline const Expression* getArray() const
                {
                    return m_array;
                }

                inline const Expression* getIndex() const
                {
                    return m_index;
                }

                Expression::Type getExpressionType() const
                {
                    return Type::ArrayIndex;
                }

            private:

                Expression* m_array;
                Expression* m_index;
            };
        }
    }
}
