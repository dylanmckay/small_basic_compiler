#pragma once

#include "Statement.hpp"

#include "../expressions/Expression.hpp"

#include <vector>

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class WhileStatement : public Statement
            {

            public:

                inline WhileStatement(expressions::Expression* expression, const std::vector<Statement*>& statements)
                {
                    expression->grab();

                    m_expression = expression;

                    for(unsigned int i=0; i<statements.size(); i++)
                    {
                        statements[i]->grab();
                        m_statements.push_back(statements[i]);
                    }
                }

                ~WhileStatement()
                {
                    m_expression->drop();

                    for(unsigned int i=0; i<m_statements.size(); i++)
                        m_statements[i]->drop();
                }

                Statement::Type getStatementType() const
                {
                    return Type::While;
                }

                inline const expressions::Expression* getExpression() const { return m_expression; }

                inline const std::vector<Statement*>& getStatements() const { return m_statements; }

            private:

                expressions::Expression* m_expression;
                std::vector<Statement*> m_statements;
            };
        }
    }
}
