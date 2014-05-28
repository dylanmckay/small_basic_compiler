#include "../../../include/epsilon/codegen/ClangHelper.hpp"

#include <llvm/Support/IRReader.h>
#include <llvm/LLVMContext.h>

using namespace epsilon;
using namespace codegen;

using namespace llvm;

ClangHelper ClangHelper::LoadIRFile(const std::string& path)
{
    SMDiagnostic diag;

    Module* module = ParseIRFile(path, diag, getGlobalContext());

    return ClangHelper(*module);
}

ClangHelper::ClangHelper(llvm::Module& module) : m_module(module)
{

}

llvm::StructType* ClangHelper::getStruct(const std::string& name)
{
    return m_module.getTypeByName("struct." + name);
}

llvm::Function* ClangHelper::getFunction(const std::string& name)
{
    return m_module.getFunction(name);
}
