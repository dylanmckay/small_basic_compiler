#pragma once

#include "Statement.hpp"

#include <string>

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class LabelStatement : public Statement
            {

            public:

                inline LabelStatement(const std::string& name) : m_name(name) { }

                LabelStatement* clone() const
                {
                    return new LabelStatement(m_name);
                }

                Statement::Type getStatementType() const
                {
                    return Type::Label;
                }

                inline const std::string& getLabelName() const { return m_name; }

            private:

                std::string m_name;
            };
        }
    }
}
