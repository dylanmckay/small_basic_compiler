#pragma once

#include "Statement.hpp"

#include <string>
#include <vector>

#include <iostream>

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class FunctionCallStatement : public Statement
            {

            public:

                inline FunctionCallStatement(const std::string& name,
                                             const std::vector<expressions::Expression*>& args) : m_name(name)
                {
                    for(unsigned int i=0; i<args.size(); i++)
                    {
                        args[i]->grab();
                        m_args.push_back(args[i]);
                    }
                }

                ~FunctionCallStatement()
                {
                    for(unsigned int i=0; i<m_args.size(); i++)
                        m_args[i]->drop();
                }

                Statement::Type getStatementType() const
                {
                    return Type::FunctionCall;
                }

                const std::string& getFunctionName() const { return m_name; }

                const std::vector<expressions::Expression*>& getArguments() const { return m_args; }

            private:

                std::string m_name;
                std::vector<expressions::Expression*> m_args;
            };
        }
    }
}
