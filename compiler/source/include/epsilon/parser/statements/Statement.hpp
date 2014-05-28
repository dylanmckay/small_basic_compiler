#pragma once

#include "../../IReferenceCounted.hpp"

namespace epsilon
{
    namespace parser
    {
        namespace statements
        {
            class Statement : public IReferenceCounted
            {

            public:

                enum class Type
                {
                    Assignment,
                    ElseIf,
                    For,
                    FunctionCall,
                    Goto,
                    If,
                    Label,
                    Subroutine,
                    While
                };

                virtual inline ~Statement() { }

                virtual Statement::Type getStatementType() const = 0;

            };
        }
    }
}
