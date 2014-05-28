#pragma once

#include "Statement.hpp"

#include "../expressions/Expression.hpp"

#include <string>
#include <vector>

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class ForStatement : public Statement
            {

            public:

                inline ForStatement(const std::string& varName,
                                    expressions::Expression* initVal,
                                    expressions::Expression* top,
                                    const std::vector<Statement*>& statements,
                                    expressions::Expression* step = nullptr) : m_varName(varName) // TODO: reorganize.
                {
                    initVal->grab();
                    top->grab();

                    m_initVal = initVal;
                    m_top = top;

                    for(unsigned int i=0; i<statements.size(); i++)
                    {
                        statements[i]->grab();
                        m_statements.push_back(statements[i]);
                    }

                    if(step)
                    {
                        step->grab();
                        m_step = step;
                    }
                    else
                        m_step = nullptr;
                }

                ~ForStatement()
                {
                    m_initVal->drop();
                    m_top->drop();

                    for(unsigned int i=0; i<m_statements.size(); i++)
                        m_statements[i]->drop();

                    if(m_step)
                        m_step->drop();
                }

                Statement::Type getStatementType() const
                {
                    return Type::For;
                }

                inline const std::string& getVariableName() const { return m_varName; }

                inline const expressions::Expression* getInitializer() const { return m_initVal; }

                inline const expressions::Expression* getTop() const { return m_top; }

                /*!
                 * Gets the step of the for loop.
                 * \return The expression represting the step, or null if the step is not specified.
                 */
                inline const expressions::Expression* getStep() const { return m_step; }

                inline const std::vector<Statement*>& getStatements() const { return m_statements; }

            private:

                std::string              m_varName;
                expressions::Expression* m_initVal;
                expressions::Expression* m_top;
                expressions::Expression* m_step;
                std::vector<Statement*>  m_statements;
            };
        }
    }
}
