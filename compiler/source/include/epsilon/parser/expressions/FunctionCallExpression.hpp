#pragma once

#include "Expression.hpp"

#include <string>
#include <vector>

namespace epsilon
{
    namespace parser
    {
        namespace expressions
        {
            class FunctionCallExpression : public Expression
            {

            public:

                inline FunctionCallExpression(const std::string& name,
                                              const std::vector<Expression*>& args) : m_name(name)
                {
                    for(unsigned int i=0; i<args.size(); i++)
                    {
                        args[i]->grab();
                        m_args.push_back(args[i]);
                    }
                }

                ~FunctionCallExpression()
                {
                    for(unsigned int i=0; i<m_args.size(); i++)
                        m_args[i]->drop();
                }

                const std::string& getFunctionName() const
                {
                    return m_name;
                }

                const std::vector<Expression*>& getArguments() const
                {
                    return m_args;
                }

                Expression::Type getExpressionType() const
                {
                    return Type::FunctionCall;
                }


            private:

                std::string m_name;
                std::vector<Expression*> m_args;
            };
        }
    }
}
