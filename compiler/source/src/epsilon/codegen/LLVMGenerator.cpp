#include "../../../include/epsilon/codegen/LLVMGenerator.hpp"


#include "../../../include/epsilon/codegen/StandardLibrary.hpp"


#include <algorithm>

#include <llvm/LLVMContext.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/InstrTypes.h>
#include <llvm/Assembly/PrintModulePass.h>
#include "llvm/Analysis/Verifier.h"
#include <llvm/Support/IRReader.h>
#include <llvm/Linker.h>

using namespace epsilon;
using namespace codegen;
using namespace parser;
using namespace expressions;
using namespace statements;

using namespace llvm;
using namespace std;


const std::string LibFuncPrefix = "__SB__";
const std::string LibCtorPrefix = "__SBC__";
const std::string LibPeriodText = "LLLL";


LLVMGenerator::LLVMGenerator()
{
    m_module = nullptr;
}

LLVMGenerator::~LLVMGenerator()
{
    for(unsigned int i=0; i<m_statements.size(); i++)
        m_statements[i]->drop();
}

void LLVMGenerator::processStatement(parser::statements::Statement* statement)
{
    statement->grab();

    m_statements.push_back(statement);
}

llvm::Module* LLVMGenerator::generateCode()
{
    const static vector<Type*> MainFuncTypes;
    std::vector<Function*> LibInitFunctions;

    LLVMContext& context = getGlobalContext();

    // Create the module.
    m_module = new Module("main", context);
    m_dbgBuilder = new DIBuilder(*m_module);

    m_dbgBuilder->createCompileUnit(3, "file.sb", "path", "compiler", false, "demflags", 1);
    m_dbgFile = m_dbgBuilder->createFile("file.sb", "path");
    m_dbgDesc.push(m_dbgFile);


    linkLibrary("stdlib.bc");
    //linkLibrary("libc.bc");

    m_primitiveHelper = new PrimitiveHelper(*m_module);


    {
        Module::FunctionListType& funcList = m_module->getFunctionList();

        Module::FunctionListType::iterator it;

        for(it = funcList.begin(); it!=funcList.end(); ++it)
        {
            Function* func = it;

            std::string name(func->getName().data());

            if(name.compare(0, LibFuncPrefix.length(), LibFuncPrefix) == 0) // It's a regular library function.
            {
                name = name.substr(LibFuncPrefix.length(), name.length()-LibFuncPrefix.length()); // Remove the library function prefix.

                size_t start_pos = 0;
                while((start_pos = name.find(LibPeriodText, start_pos)) != std::string::npos)
                {
                    name.replace(start_pos, LibPeriodText.length(), ".");
                    start_pos += 1;
                }

                addFunction(name, func);
            }
            else if(name.compare(0, LibCtorPrefix.length(), LibCtorPrefix) == 0) // It's a constructing function.
            {
                LibInitFunctions.push_back(func);
            }
        }
    }


    // Declare the initialize vars function.
    FunctionType* initVarsFuncType = FunctionType::get(Type::getVoidTy(context), std::vector<Type*>{ }, false);
    m_initVarsFunc = Function::Create(initVarsFuncType, GlobalValue::ExternalLinkage, "SBInitVars", m_module);

    FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(context), MainFuncTypes, false);

    // Create the main function.
    m_mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "SBMain", m_module);

    // Push the function stack.
    m_functionStack.push(m_mainFunc);

    m_mainFunc->setCallingConv(CallingConv::C);

    // Create the entry basic block for the main function.
    BasicBlock* mainbblock = BasicBlock::Create(context, "entry", m_mainFunc, 0);

    m_basicBlocks.push(mainbblock);

    // Call the initialize variables function.
    CallInst::Create(m_initVarsFunc, std::vector<Value*>{}, "", mainbblock);

    for(unsigned int i=0; i<LibInitFunctions.size(); i++)
    {
        CallInst::Create(LibInitFunctions[i], std::vector<Value*>{}, "", mainbblock);
    }

    for(unsigned int i=0; i<m_statements.size(); i++)
    {
        emitStatement(m_statements[i]);

        m_statements[i]->drop();
    }

    while(m_basicBlocks.size() > 0)
    {
        ReturnInst::Create(getGlobalContext(), m_basicBlocks.top());

        m_basicBlocks.pop();
    }

    // Return from the 'entry' block in main if there is no existing terminator.
    //if(!mainbblock->getTerminator())
    //    ReturnInst::Create(context, getCurrentBBlock());


    {
        BasicBlock* initbblock = BasicBlock::Create(context, "entry", m_initVarsFunc, 0);

        std::map<std::string, Value*>::iterator it;

        for(it = m_variables.begin(); it != m_variables.end(); it++)
        {
            //m_primitiveHelper->emitCopy(it->second, defaultPrimitive, initbblock);
            Value* val = new LoadInst(m_primitiveHelper->getConstantPrimitive("", initbblock), "", initbblock);
            new StoreInst(val, it->second, initbblock);
        }

        ReturnInst::Create(context, initbblock);
    }

    m_dbgBuilder->finalize();

    if(!verify())
    {
        throw std::runtime_error("internal error: could not verify IR");
    }

    //delete m_dbgBuilder;

    return m_module;

}

bool LLVMGenerator::verify()
{
    return !verifyModule(*m_module, PrintMessageAction);
}

void LLVMGenerator::addFunction(const std::string& name, llvm::Function* func)
{
    m_functions.insert(std::pair<std::string,Function*>(name, func));
}

void LLVMGenerator::linkLibrary(llvm::Module& mod)
{
    std::string err;

    bool result = !Linker::LinkModules(m_module, &mod, Linker::DestroySource, &err);

    if(!result)
        throw std::runtime_error(err);
}

void LLVMGenerator::linkLibrary(const std::string& path)
{
    SMDiagnostic diag;

    Module* mod = ParseIRFile(path, diag, getGlobalContext());

    if(!mod)
        throw std::runtime_error("could not load library '" + path + "': " + diag.getMessage());

    linkLibrary(*mod);
}

llvm::Value* LLVMGenerator::getOrCreateVariablePointer(const std::string& name)
{
    std::map<std::string, llvm::Value*>::iterator it;

    it = m_variables.find(name);

    // The variable does not exist.
    if(it == m_variables.end())
    {
        //throw std::runtime_error("implement");
        GlobalVariable* global = new GlobalVariable(*m_module,
                                                    m_primitiveHelper->getPrimitiveType(), // Primitive.
                                                    false, // Not constant
                                                    GlobalValue::ExternalLinkage,
                                                    Constant::getNullValue(m_primitiveHelper->getPrimitiveType()), // Null initializer.
                                                    name);

        // Add the new variable.
        m_variables.insert(std::pair<std::string, llvm::Value*>(name, global));

        return global;

    }
    else // The variable does exist.
    {
        return it->second;
    }
}

llvm::Value* LLVMGenerator::emitCallFunction(llvm::Function* func, const std::vector<parser::expressions::Expression*>& args)
{

    if(func->getArgumentList().size() != args.size())
    {
        std::ostringstream stream;

        stream << "function expects " << func->getArgumentList().size() << " arguments";

        throw std::runtime_error(stream.str());
    }

    std::vector<Value*> argVals;


    for(unsigned int i=0; i<args.size(); i++)
    {
        argVals.push_back(getExpressionValue(args[i]));
    }

    return CallInst::Create(func, argVals, "", getCurrentBBlock());
}

llvm::Function* LLVMGenerator::getFunction(const std::string& name)
{
    std::map<std::string,llvm::Function*>::iterator it;

    // Search for the function.
    it = m_functions.find(name);

    // Check if it was found.
    if(it != m_functions.end())
        return it->second;
    else // It wasn't found.
        return nullptr;
}

bool LLVMGenerator::isExpressionModifiable(const parser::expressions::Expression* expr)
{
    Expression::Type type = expr->getExpressionType();

    while(type == Expression::Type::ArrayIndex)
    {
        expr = ((ArrayIndexExpression*)expr)->getArray();
        type = expr->getExpressionType();
    }

    // Variables are modifiable.
    if(type == Expression::Type::Variable)
        return true;
    else
        return false;
}

void LLVMGenerator::emitStatement(const parser::statements::Statement* statement)
{
    switch(statement->getStatementType())
    {
        case Statement::Type::Assignment:
        {
            emitStatement((AssignmentStatement*)statement);
            break;
        }
        case Statement::Type::For:
        {
            emitStatement((ForStatement*)statement);
            break;
        }
        case Statement::Type::FunctionCall:
        {
            emitStatement((FunctionCallStatement*)statement);
            break;
        }
        case Statement::Type::If:
        {
            emitStatement((IfStatement*)statement);
            break;
        }
        case Statement::Type::Label:
        {
            emitStatement((LabelStatement*)statement);
            break;
        }
        case Statement::Type::Subroutine:
        {
            emitStatement((SubroutineStatement*)statement);
            break;
        }
        case Statement::Type::While:
        {
            emitStatement((WhileStatement*)statement);
            break;
        }
        default:
        {
            throw std::runtime_error("LLVM generator: statement not implemented");
        }
    }
/*
    // Check if we need to destroy any primitives.
    if(m_primitivesToDestroy.size() > 0)
    {
        // Loop the primitives to destroy.
        while(m_primitivesToDestroy.size() != 0)
        {
            // Destroy it.
            m_primitiveHelper->emitDestroyPrimitive(m_primitivesToDestroy.top(), getCurrentBBlock());

            // Pop it from the stack.
            m_primitivesToDestroy.pop();
        }
    }*/
}

/*!
 * Emit an assignment statement.
 */
void LLVMGenerator::emitStatement(const parser::statements::AssignmentStatement* statement)
{
    Value* rhsVal = getExpressionValue(statement->getRHS());

    // A regular variable expression.
    if(statement->getLHS()->getExpressionType() == Expression::Type::Variable)
    {
        VariableExpression* varExpr = (VariableExpression*)statement->getLHS();

        Value* lhsVal = getOrCreateVariablePointer(varExpr->getVariableName());
        m_primitiveHelper->emitCopy(lhsVal, rhsVal, getCurrentBBlock());
    }
    else if(statement->getLHS()->getExpressionType() == Expression::Type::ArrayIndex)
    {
        ArrayIndexExpression* arrayExpr = (ArrayIndexExpression*)statement->getLHS();

        // Expression is trying to index an item in a function return type or something.
        if(arrayExpr->getArray()->getExpressionType() != Expression::Type::Variable)
        {
            throw std::runtime_error("you can only assign values to arrays which are stored in user variables");
        }

        Value* arrayVal = getOrCreateVariablePointer(((VariableExpression*)arrayExpr->getArray())->getVariableName());
        Value* keyVal   = getExpressionValue(arrayExpr->getIndex());

        m_primitiveHelper->emitSetElement(arrayVal, keyVal, rhsVal, getCurrentBBlock());
    }
    else
    {
        throw std::runtime_error("only variables (or variable array index expressions) may be the target of an assignment");
    }


}

void LLVMGenerator::emitStatement(const parser::statements::ForStatement* statement)
{
    // The statements inside the loop.
    const std::vector<Statement*>& statements = statement->getStatements();

    BasicBlock* bodyBlock  = BasicBlock::Create(getGlobalContext(), "forbody", getCurrentFunction());
    BasicBlock* afterBlock = BasicBlock::Create(getGlobalContext(), "afterfor", getCurrentFunction());

    // The variable that is to be changed.
    Value* var         = getOrCreateVariablePointer(statement->getVariableName());
    Value* initializer = getExpressionValue(statement->getInitializer());
    Value* top         = getExpressionValue(statement->getTop());
    Value* step;

    // Check if a loop step is specified.
    if(statement->getStep())
        step = getExpressionValue(statement->getStep()); // Set the step to the one provided.
    else // No loop step specified.
        step = m_primitiveHelper->getConstantPrimitive(1, getCurrentBBlock()); // Step = 1.

    // Copy the initializer to the var.
    m_primitiveHelper->emitCopy(var, initializer, getCurrentBBlock());

    // Get a value (boolean) containing if the variable is less than or equal to the top value.
    Value* boolExpr = m_primitiveHelper->emitToBoolean(m_primitiveHelper->emitLessThanOrEqualTo(var, top, getCurrentBBlock()), getCurrentBBlock());

    // If the value is less than the top value, execute the for loop, otherwise break to 'after' block.
    BranchInst::Create(bodyBlock, afterBlock, boolExpr, getCurrentBBlock());

    // Stop emitting to the block we just branched from.
    m_basicBlocks.pop();

    // Generate code for the body.
    m_basicBlocks.push(bodyBlock);

    // Emit the for loop body.
    for(unsigned int i=0; i<statements.size(); i++)
    {
        emitStatement(statements[i]);
    }

    // Increment the variable by the step and store it back into the variable.
    m_primitiveHelper->emitCopy(var, m_primitiveHelper->emitAdd(var, step, getCurrentBBlock()), getCurrentBBlock());

    // Re-evaluate the boolean "is var less than top" value.
    boolExpr = m_primitiveHelper->emitToBoolean(m_primitiveHelper->emitLessThanOrEqualTo(var, top, getCurrentBBlock()), getCurrentBBlock());

    // Branch from the for body block (which may have been changed by emitting statements) to the after block.
    // Or branch to the 'after' block if the for loop has finished.
    BranchInst::Create(bodyBlock, afterBlock, boolExpr, getCurrentBBlock());

    // Stop emitting to the body block (or whatever block it transferred control to).
    m_basicBlocks.pop();

    // Start emitting to the 'after' block.
    m_basicBlocks.push(afterBlock);

}

void LLVMGenerator::emitStatement(const parser::statements::FunctionCallStatement* statement)
{
    Function* func = getFunction(statement->getFunctionName());

    if(!func)
    {
        throw std::runtime_error("function \"" + statement->getFunctionName() + "\" does not exist");
    }

    emitCallFunction(func, statement->getArguments());
}

void LLVMGenerator::emitStatement(const parser::statements::IfStatement* statement)
{
    BasicBlock* thenBlock  = BasicBlock::Create(getGlobalContext(), "then", getCurrentFunction());
    BasicBlock* elseBlock  = BasicBlock::Create(getGlobalContext(), "else", getCurrentFunction());
    BasicBlock* mergeBlock = BasicBlock::Create(getGlobalContext(), "merge", getCurrentFunction());

    Value* boolCond = m_primitiveHelper->emitToBoolean(getExpressionValue(statement->getExpression()), getCurrentBBlock());

    // Emit the branch to the if or else block.
    BranchInst::Create(thenBlock, elseBlock, boolCond, getCurrentBBlock());

    m_basicBlocks.pop(); // We don't want to use the old basic block anymore.

    // Get the statements of the if branch.
    const std::vector<Statement*>& trueStatements = statement->getTrueStatements();
    const std::vector<Statement*>& falseStatements = statement->getFalseStatements();

    // Generate code to the then block.
    m_basicBlocks.push(thenBlock);

    // Emit the true statements.
    for(unsigned int i=0; i<trueStatements.size(); i++)
    {
        emitStatement(trueStatements[i]);
    }

    // Branch from the true block (which may have been changed by emitting statements) to the merge block.
    BranchInst::Create(mergeBlock, getCurrentBBlock());

    // Stop emitting to the true block.
    m_basicBlocks.pop();

    // Start emitting to the else block.
    m_basicBlocks.push(elseBlock);

    // Emit the else statements.
    for(unsigned int i=0; i<falseStatements.size(); i++)
    {
        emitStatement(falseStatements[i]);
    }

    // Branch from the false block (which may have been changed by emitting statements) to the merge block.
    BranchInst::Create(mergeBlock, getCurrentBBlock());

    // Stop emitting to the else block.
    m_basicBlocks.pop();

    // We will now generate code into the merge block.
    m_basicBlocks.push(mergeBlock);
}

/*!
 * Emits a label statement.
 */
void LLVMGenerator::emitStatement(const parser::statements::LabelStatement* statement)
{
    throw std::runtime_error("implement");
}

/*!
 * Emit a subroutine statement.
 */
void LLVMGenerator::emitStatement(const parser::statements::SubroutineStatement* statement)
{
    std::string name = statement->getSubroutineName();

    // Get a function type which returns nothing and takes no parameters,
    FunctionType* funcType = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);

    // Create the main function.
    Function* func = Function::Create(funcType, GlobalValue::ExternalLinkage, name, m_module);

    // Push the function.
    m_functionStack.push(func);

    addFunction(name, func); // Add the function to the program.

    // Create the entry basic block for the main function.
    BasicBlock* bblock = BasicBlock::Create(getGlobalContext(), "entry", func, 0);

    // Push the new block onto the stack.
    m_basicBlocks.push(bblock);

    const std::vector<Statement*>& statements = statement->getStatements();

    // Emit the statements inside the subroutine.
    for(unsigned int i=0; i<statements.size(); i++)
    {
        emitStatement(statements[i]);
    }

    // Return from the basic block.
    ReturnInst::Create(getGlobalContext(), getCurrentBBlock());

    // Pop the basic block.
    m_basicBlocks.pop();

    // Pop the function.
    m_functionStack.pop();

    DIArray array = m_dbgBuilder->getOrCreateArray(std::vector<Value*>{});
    DIType type = m_dbgBuilder->createSubroutineType(m_dbgFile, array);


    m_dbgBuilder->createFunction(m_dbgDesc.top(), name, name, m_dbgFile, 3, type, false, true, 8);
}

void LLVMGenerator::emitStatement(const parser::statements::WhileStatement* statement)
{
    const Expression* expression = statement->getExpression();
    const std::vector<Statement*>& statements = statement->getStatements();

    BasicBlock* afterBB = BasicBlock::Create(getGlobalContext(), "wafter", getCurrentFunction());
    BasicBlock* bodyBB  = BasicBlock::Create(getGlobalContext(), "wbody", getCurrentFunction(), afterBB);

    // Evaluate the condition.
    Value* boolVal = m_primitiveHelper->emitToBoolean(getExpressionValue(expression), getCurrentBBlock());

    // Branch to the after loop if the expression is false at the beginning, or jump to the while body if it is true.
    BranchInst::Create(bodyBB, afterBB, boolVal, getCurrentBBlock());

    // Don't generate code for the previous basic block anymore.
    m_basicBlocks.pop();

    // Generate code for the while 'body' block.
    m_basicBlocks.push(bodyBB);

    // Emit the while body/
    for(unsigned int i=0; i<statements.size(); i++)
    {
        emitStatement(statements[i]);
    }

    // Evaluate the condition again and branch back to loop if condition is true, otherwise branch to 'after' block.
    boolVal = m_primitiveHelper->emitToBoolean(getExpressionValue(expression), getCurrentBBlock());
    BranchInst::Create(bodyBB, afterBB, boolVal, getCurrentBBlock());

    // Stop generating code for the while 'body'.
    m_basicBlocks.pop();

    // We will not generate code to the 'after' block.
    m_basicBlocks.push(afterBB);
}

llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::Expression* expr)
{
    switch(expr->getExpressionType())
    {
        case Expression::Type::Equal:
        {
            return getExpressionValue((EqualExpression*)expr);
        }
        case Expression::Type::NotEqual:
        {
            return getExpressionValue((NotEqualExpression*)expr);
        }
        case Expression::Type::Addition:
        {
            return getExpressionValue((AdditionExpression*)expr);
        }
        case Expression::Type::Subtraction:
        {
            return getExpressionValue((SubtractionExpression*)expr);
        }
        case Expression::Type::Multipication:
        {
            return getExpressionValue((MultipicationExpression*)expr);
        }
        case Expression::Type::Division:
        {
            return getExpressionValue((DivisionExpression*)expr);
        }
        case Expression::Type::LessThan:
        {
            return getExpressionValue((LessThanExpression*)expr);
        }
        case Expression::Type::GreaterThan:
        {
            return getExpressionValue((GreaterThanExpression*)expr);
        }
        case Expression::Type::LessThanOrEqualTo:
        {
            return getExpressionValue((LessThanOrEqualToExpression*)expr);
        }
        case Expression::Type::GreaterThanOrEqualTo:
        {
            return getExpressionValue((GreaterThanOrEqualToExpression*)expr);
        }
        case Expression::Type::And:
        {
            return getExpressionValue((AndExpression*)expr);
        }
        case Expression::Type::Or:
        {
            return getExpressionValue((OrExpression*)expr);
        }
        case Expression::Type::Negative:
        {
            return getExpressionValue((NegativeExpression*)expr);
        }
        /*case Expression::Type::Not
        {
            return getExpressionValue((NotExpression*)expr);
        }*/
        case Expression::Type::NumericLiteral:
        {
            return getExpressionValue((NumericLiteralExpression*)expr);
        }
        case Expression::Type::StringLiteral:
        {
            return getExpressionValue((StringLiteralExpression*)expr);
        }
        case Expression::Type::Variable:
        {
            return getExpressionValue((VariableExpression*)expr);
        }
        case Expression::Type::FunctionCall:
        {
            return getExpressionValue((FunctionCallExpression*)expr);
        }
        default:
        {
            throw std::runtime_error("LLVM generator: expression not implemented");
        }
    }
}

/*!
 * Get the result of an equals expression.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::EqualExpression* expr)
{
    Value* ret = m_primitiveHelper->emitIsEqual(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Get the result of a mot equal expression.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::NotEqualExpression* expr)
{
    Value* ret = m_primitiveHelper->emitIsNotEqual(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Get the result of a primitive addition.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::AdditionExpression* expr)
{
    Value* ret = m_primitiveHelper->emitAdd(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Get the result of a primitive subtraction.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::SubtractionExpression* expr)
{
    Value* ret = m_primitiveHelper->emitSubtract(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Get the result of a primitive multipication.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::MultipicationExpression* expr)
{
    Value* ret = m_primitiveHelper->emitMultiply(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Get the result of a primitive division.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::DivisionExpression* expr)
{
    Value* ret = m_primitiveHelper->emitDivide(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Gets the value of a less than expression.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::LessThanExpression* expr)
{
    Value* ret = m_primitiveHelper->emitLessThan(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Gets the value of a greater than expression.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::GreaterThanExpression* expr)
{
    Value* ret = m_primitiveHelper->emitGreaterThan(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Gets the value of a less than or equal to expression.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::LessThanOrEqualToExpression* expr)
{
    Value* ret = m_primitiveHelper->emitLessThanOrEqualTo(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Gets the value of a greater than or equal toexpression.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::GreaterThanOrEqualToExpression* expr)
{
    Value* ret = m_primitiveHelper->emitGreaterThanOrEqualTo(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::AndExpression* expr)
{
    return m_primitiveHelper->emitAnd(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());
}

llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::OrExpression* expr)
{
    return m_primitiveHelper->emitOr(getExpressionValue(expr->getLHS()), getExpressionValue(expr->getRHS()), getCurrentBBlock());
}

llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::NegativeExpression* expr)
{
    return m_primitiveHelper->emitNegative(getExpressionValue(expr->getTarget()), getCurrentBBlock());
}

/*!
 * Get a numeric literal primitive.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::NumericLiteralExpression* expr)
{
    return m_primitiveHelper->getConstantPrimitive(expr->getValue(), getCurrentBBlock());
}

/*!
 * Get a string literal primitive.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::StringLiteralExpression* expr)
{
    Value* ret = m_primitiveHelper->getConstantPrimitive(expr->getValue(), getCurrentBBlock());

    m_primitivesToDestroy.push(ret);

    return ret;
}

/*!
 * Get a variable primitive.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::VariableExpression* expr)
{
    return getOrCreateVariablePointer(expr->getVariableName());
}

/*!
 * Gets the result of a function call.
 */
llvm::Value* LLVMGenerator::getExpressionValue(const parser::expressions::FunctionCallExpression* expr)
{
    Function* func = getFunction(expr->getFunctionName());

    if(!func)
    {
        throw std::runtime_error("function \"" + expr->getFunctionName() + "\" does not exist");
    }

    return emitCallFunction(func, expr->getArguments());
}
