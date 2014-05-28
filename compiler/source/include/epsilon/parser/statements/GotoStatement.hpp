#pragma once

#include "Statement.hpp"

#include <string>

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class GotoStatement : public Statement
            {

            public:

                inline GotoStatement(const std::string& labelName) : m_labelName(labelName) { }


                Statement::Type getStatementType() const
                {
                    return Type::Goto;
                }

            private:

                std::string m_labelName;
            };
        }
    }
}
