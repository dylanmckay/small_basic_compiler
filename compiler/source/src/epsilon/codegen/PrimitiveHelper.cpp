#include "../../../include/epsilon/codegen/PrimitiveHelper.hpp"

#include <llvm/LLVMContext.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/Linker.h>
#include <llvm/Support/IRReader.h>

#include <sstream>

using namespace epsilon;
using namespace codegen;

using namespace llvm;


PrimitiveHelper::PrimitiveHelper(llvm::Module& module) : m_module(module)
{
    ClangHelper helper(module);

    m_primitiveType = helper.getStruct("Primitive");

    if(!m_primitiveType)
        throw std::runtime_error("invalid standard library, must have 'Primitive' structure");

    m_constructStrFunc = loadStdFunc("CreatePrimitiveFromString");

    m_constructDblFunc = loadStdFunc("CreatePrimitiveFromDouble");

    m_copyPrimitiveFunc = loadStdFunc("CopyPrimitive");

    m_destroyPrimitiveFunc = loadStdFunc("DestroyPrimitive");

    m_setElemFunc = loadStdFunc("PrimitiveSetElement");

    m_isEqualFunc = loadStdFunc("PrimitiveDoesEqual");

    m_isNotEqualFunc = loadStdFunc("PrimitiveDoesNotEqual");

    m_toBoolFunc = loadStdFunc("PrimitiveToBoolean");

    m_addFunc = loadStdFunc("AddPrimitive");

    m_subFunc = loadStdFunc("SubtractPrimitive");

    m_mulFunc = loadStdFunc("MultiplyPrimitive");

    m_divFunc = loadStdFunc("DividePrimitive");

    m_lessThanFunc = loadStdFunc("PrimitiveLessThan");

    m_greaterThanFunc = loadStdFunc("PrimitiveGreaterThan");

    m_lessThanOrEqualToFunc = loadStdFunc("PrimitiveLessThanOrEqualTo");

    m_greaterThanOrEqualToFunc = loadStdFunc("PrimitiveGreaterThanOrEqualTo");

    m_andFunc = loadStdFunc("PrimitiveAnd");

    m_orFunc = loadStdFunc("PrimitiveOr");

    m_notFunc = loadStdFunc("PrimitiveNot");

    m_negativeFunc = loadStdFunc("PrimitiveNegative");

}

void PrimitiveHelper::emitCopy(llvm::Value* dest, llvm::Value* src, llvm::BasicBlock* bb)
{
    CallInst::Create(m_copyPrimitiveFunc, std::vector<Value*>{dest, src}, "", bb);
}

void PrimitiveHelper::emitSetElement(llvm::Value* array, llvm::Value* key, llvm::Value* value, llvm::BasicBlock* bb)
{
    CallInst::Create(m_setElemFunc, std::vector<Value*>{array,key,value}, "", bb);
}

llvm::Value* PrimitiveHelper::emitIsEqual(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_isEqualFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitIsNotEqual(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_isNotEqualFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitToBoolean(llvm::Value* prim, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_toBoolFunc, std::vector<Value*> {prim}, "", bb);
}

llvm::Value* PrimitiveHelper::emitAdd(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_addFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitSubtract(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_subFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitMultiply(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_mulFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitDivide(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_divFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitLessThan(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_lessThanFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitGreaterThan(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_greaterThanFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitLessThanOrEqualTo(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_lessThanOrEqualToFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitGreaterThanOrEqualTo(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_greaterThanOrEqualToFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitAnd(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_andFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitOr(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_orFunc, std::vector<Value*> {prim1, prim2}, "", bb);
}

llvm::Value* PrimitiveHelper::emitNot(llvm::Value* prim, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_notFunc, std::vector<Value*> {prim}, "", bb);
}

llvm::Value* PrimitiveHelper::emitNegative(llvm::Value* prim, llvm::BasicBlock* bb)
{
    return CallInst::Create(m_negativeFunc, std::vector<Value*> {prim}, "", bb);
}

llvm::Value* PrimitiveHelper::getConstantPrimitive(double val, llvm::BasicBlock* bb)
{
    //ConstantInt* int0 = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
    ConstantFP* dblConstant = ConstantFP::get(getGlobalContext(), APFloat(val));

    GlobalVariable* global = new GlobalVariable(m_module,
                                                dblConstant->getType(),
                                                true,
                                                GlobalValue::ExternalLinkage,
                                                dblConstant,
                                                "double");

    //GetElementPtrInst* dblPtr = GetElementPtrInst::Create(global, std::vector<Value*>(2, int0), "dbl", bb);

    LoadInst* loaded = new LoadInst(global, "", bb);

    CallInst* primitive = CallInst::Create(m_constructDblFunc, std::vector<Value*>(1, loaded), "primitive", bb);

    return primitive;
}

llvm::Value* PrimitiveHelper::getConstantPrimitive(const std::string& str, llvm::BasicBlock* bb)
{
    ConstantInt* int0 = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
    Constant* strConstant = ConstantDataArray::getString(getGlobalContext(), str, true);

    GlobalVariable* global = new GlobalVariable(m_module,
                                                strConstant->getType(),
                                                true,
                                                GlobalValue::ExternalLinkage,
                                                strConstant,
                                                "string");

    GetElementPtrInst* strPtr = GetElementPtrInst::Create(global, std::vector<Value*>(2, int0), "str", bb);

    CallInst* primitive = CallInst::Create(m_constructStrFunc, std::vector<Value*>(1, strPtr), "primitive", bb);

    return primitive;

}

void PrimitiveHelper::emitDestroyPrimitive(llvm::Value* val, llvm::BasicBlock* bb)
{
    CallInst::Create(m_destroyPrimitiveFunc, std::vector<Value*>(1, val), "", bb);
}

llvm::Function* PrimitiveHelper::loadStdFunc(const std::string& name)
{
    ClangHelper helper(m_module);

    Function* func = helper.getFunction(name);

    if(!func)
        throw std::runtime_error("standard library must implement " + name);

    return func;
}
