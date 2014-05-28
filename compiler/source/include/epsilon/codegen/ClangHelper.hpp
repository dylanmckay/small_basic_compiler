#pragma once

#include <stdint.h>

#include <llvm/Module.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>

#include <string>

namespace epsilon
{
    namespace codegen
    {
        /*!
         *
         */
        class ClangHelper
        {

        public:

            static ClangHelper LoadIRFile(const std::string& path);

            ClangHelper();

            /*!
             * Creates a clang helper from the given module.
             */
            ClangHelper(llvm::Module& module);

            /*!
             * Gets the C structure with the given name.
             * \return The structure, or null if it does not exist.
             */
            llvm::StructType* getStruct(const std::string& name);

            /*!
             * Gets a C function from the module.
             */
            llvm::Function* getFunction(const std::string& name);

            inline llvm::Module& getModule() { return m_module; }



        private:

            llvm::Module& m_module;
        };
    }
}
