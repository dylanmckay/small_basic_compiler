#pragma once

#include "../parser/Parser.hpp"
#include "PrimitiveHelper.hpp"

#include <llvm/Module.h>
#include <llvm/DIBuilder.h>
#include <llvm/DebugInfo.h>

#include <stack>
#include <vector>
#include <map>

namespace epsilon
{
    namespace codegen
    {
        class LLVMGenerator
        {

        public:

            LLVMGenerator();

            ~LLVMGenerator();

            void processStatement(parser::statements::Statement* statement);

            llvm::Module* generateCode();

            /*!
             * Verifies that the module contains errors.
             */
            bool verify();

            /*!
             * Adds a function to the list.
             */
            void addFunction(const std::string& name, llvm::Function* func);

            void linkLibrary(llvm::Module& mod);

            void linkLibrary(const std::string& path);

            inline llvm::Module* getModule() { return m_module; }

            inline PrimitiveHelper& getPrimitiveHelper() { return *m_primitiveHelper; }

            inline llvm::Function* getEntryPoint() { return m_mainFunc; }

        private:

            llvm::Module* m_module;

            llvm::DIBuilder* m_dbgBuilder;

            llvm::DIFile m_dbgFile;

            std::stack<llvm::DIDescriptor> m_dbgDesc;

            llvm::Function* m_mainFunc;

            std::stack<llvm::BasicBlock*> m_basicBlocks;

            std::stack<llvm::Function*> m_functionStack;

            std::vector<parser::statements::Statement*> m_statements;

            std::map<std::string, llvm::Value*> m_variables;

            std::map<std::string, llvm::Function*> m_functions;

            PrimitiveHelper* m_primitiveHelper;

            std::stack<llvm::Value*> m_primitivesToDestroy; // A list of primitives to destroy.

            llvm::Function* m_initVarsFunc;

            inline llvm::BasicBlock* getCurrentBBlock() { return m_basicBlocks.top(); }

            inline llvm::Function* getCurrentFunction() { return m_functionStack.top(); }

            /*!
             * Gets a variable or creates it if it does not exist.
             * \note The variable will be a pointer to the actual variable.
             */
            llvm::Value* getOrCreateVariablePointer(const std::string& name);

            llvm::Value* emitCallFunction(llvm::Function* func, const std::vector<parser::expressions::Expression*>& args);

            /*!
             * Gets the function with the given name.
             * \return The function, or 'nullptr' if it does not exist.
             */
            llvm::Function* getFunction(const std::string& name);

            /*!
             * Checks if an expression is modifiable.
             */
            bool isExpressionModifiable(const parser::expressions::Expression* expr);

            void emitStatement(const parser::statements::Statement* statement);

            void emitStatement(const parser::statements::AssignmentStatement* statement);

            void emitStatement(const parser::statements::ForStatement* statement);

            void emitStatement(const parser::statements::FunctionCallStatement* statement);

            void emitStatement(const parser::statements::IfStatement* statement);

            void emitStatement(const parser::statements::LabelStatement* statement);

            void emitStatement(const parser::statements::SubroutineStatement* statement);

            void emitStatement(const parser::statements::WhileStatement* statement);

            llvm::Value* getExpressionValue(const parser::expressions::Expression* expr);

            llvm::Value* getExpressionValue(const parser::expressions::EqualExpression* expr);

            llvm::Value* getExpressionValue(const parser::expressions::NotEqualExpression* expr);

            // Arithmetic expressions.
            llvm::Value* getExpressionValue(const parser::expressions::AdditionExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::SubtractionExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::MultipicationExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::DivisionExpression* expr);

            // Boolean expressions.
            llvm::Value* getExpressionValue(const parser::expressions::LessThanExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::GreaterThanExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::LessThanOrEqualToExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::GreaterThanOrEqualToExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::AndExpression* expr);
            llvm::Value* getExpressionValue(const parser::expressions::OrExpression* expr);

            llvm::Value* getExpressionValue(const parser::expressions::NegativeExpression* expr);

            llvm::Value* getExpressionValue(const parser::expressions::NumericLiteralExpression* expr);

            llvm::Value* getExpressionValue(const parser::expressions::StringLiteralExpression* expr);

            llvm::Value* getExpressionValue(const parser::expressions::VariableExpression* expr);

            llvm::Value* getExpressionValue(const parser::expressions::FunctionCallExpression* expr);

            void destroyUnneededPrimitives();

        };
    }
}
