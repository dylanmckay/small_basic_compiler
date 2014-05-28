#pragma once

#include "Statement.hpp"

#include "../expressions/Expression.hpp"
#include <string>

#include <iostream>

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class AssignmentStatement : public Statement
            {

            public:

                inline AssignmentStatement(expressions::Expression* var, expressions::Expression* value)
                {
                    var->grab();
                    value->grab();

                    m_var = var;
                    m_value = value;
                }

                ~AssignmentStatement()
                {
                    m_var->drop();
                    m_value->drop();
                }

                inline const expressions::Expression* getLHS() const
                {
                    return m_var;
                }

                inline const expressions::Expression* getRHS() const
                {
                    return m_value;
                }

                Statement::Type getStatementType() const
                {
                    return Type::Assignment;
                }

            private:

                expressions::Expression* m_var;
                expressions::Expression* m_value;
            };
        }
    }
}
