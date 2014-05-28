#include "../../../include/epsilon/codegen/StandardLibrary.hpp"

#include "../../../include/epsilon/codegen/LLVMGenerator.hpp"

#include <llvm/LLVMContext.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/InstrTypes.h>

using namespace epsilon;
using namespace codegen;

using namespace llvm;


void StandardLibrary::AddToGenerator(LLVMGenerator& gen)
{
    AddTextWindow(gen);
}

void StandardLibrary::AddTextWindow(LLVMGenerator& gen)
{
/*
    PrimitiveHelper& primitiveHelper = gen.getPrimitiveHelper();

    std::vector<Type*> writeLineParams(1, primitiveHelper.getPrimitiveType());

    FunctionType* writeLinefuncType = FunctionType::get(Type::getVoidTy(getGlobalContext()), writeLineParams, false);

    FunctionType* putsFuncType = FunctionType::get(Type::getInt32Ty(getGlobalContext()), std::vector<Type*> { Type::getInt8PtrTy(getGlobalContext()) }, false);
    Function* putsFunc = Function::Create(putsFuncType, GlobalValue::ExternalLinkage, "puts", gen.getModule());

    Function* writeLineFunc = Function::Create(writeLinefuncType, GlobalValue::ExternalLinkage, "TextWindow.WriteLine", gen.getModule());

    BasicBlock* bblock = BasicBlock::Create(getGlobalContext(), "entry", writeLineFunc, 0);



    CallInst::Create(putsFunc, std::vector<Value*> { primitiveHelper.getCStringFromPrimitive(&*writeLineFunc->getArgumentList().begin(), bblock) }, "", bblock);


    ReturnInst::Create(getGlobalContext(), bblock);

*/



    //gen.addFunction("TextWindow.WriteLine", writeLineFunc);
}
