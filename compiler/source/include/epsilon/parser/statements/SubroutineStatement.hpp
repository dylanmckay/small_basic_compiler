#pragma once

#include "Statement.hpp"

#include <string>
#include <vector>

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class SubroutineStatement : public Statement
            {

            public:

                inline SubroutineStatement(const std::string& name,
                                           const std::vector<Statement*>& statements) : m_name(name)
                {
                    for(unsigned int i=0; i<statements.size(); i++)
                    {
                        statements[i]->grab();
                        m_statements.push_back(statements[i]);
                    }
                }

                ~SubroutineStatement()
                {
                    for(unsigned int i=0; i<m_statements.size(); i++)
                        m_statements[i]->drop();
                }

                Statement::Type getStatementType() const
                {
                    return Type::Subroutine;
                }

                inline const std::string& getSubroutineName() const { return m_name; }

                inline const std::vector<Statement*>& getStatements() const { return m_statements; }

            private:

                std::string m_name;
                std::vector<Statement*> m_statements;
            };
        }
    }
}
