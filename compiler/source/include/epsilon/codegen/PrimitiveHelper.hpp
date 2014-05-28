#pragma once

#include <stdint.h>
#include <llvm/Module.h>

#include "ClangHelper.hpp"

namespace epsilon
{
    namespace codegen
    {
        class PrimitiveHelper
        {

        public:

            /*!
             * Creates a primitive helper from the given module.
             */
            PrimitiveHelper(llvm::Module& module);

            void emitCopy(llvm::Value* dest, llvm::Value* src, llvm::BasicBlock* bb);

            void emitSetElement(llvm::Value* array, llvm::Value* key, llvm::Value* value, llvm::BasicBlock* bb);

            llvm::Value* emitIsEqual(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitIsNotEqual(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitToBoolean(llvm::Value* prim, llvm::BasicBlock* bb);



            /*!
             * Adds the two primitive values together and returns the result as an LLVM value.
             */
            llvm::Value* emitAdd(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitSubtract(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitMultiply(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitDivide(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitLessThan(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitGreaterThan(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitLessThanOrEqualTo(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitGreaterThanOrEqualTo(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitAnd(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);
            llvm::Value* emitOr(llvm::Value* prim1, llvm::Value* prim2, llvm::BasicBlock* bb);

            llvm::Value* emitNot(llvm::Value* prim, llvm::BasicBlock* bb);

            llvm::Value* emitNegative(llvm::Value* prim, llvm::BasicBlock* bb);


            /*!
             * Gets a constant primitive for the primitive which equals 'val'.
             */
            llvm::Value* getConstantPrimitive(double val, llvm::BasicBlock* bb);

            /*!
             * Gets a constant primitive for the primitive which is a string.
             */
            llvm::Value* getConstantPrimitive(const std::string& str, llvm::BasicBlock* bb);

            /*!
             * Destroys a primitive.
             */
            void emitDestroyPrimitive(llvm::Value* val, llvm::BasicBlock* bb);

            /*!
             * Gets the primitive type.
             */
            inline llvm::Type* getPrimitiveType() { return m_primitiveType; }

        private:

            llvm::Module& m_module;

            llvm::StructType* m_primitiveType;

            unsigned int m_indexToStr; // The element number of the char pointer in the type.


            llvm::Function* m_constructStrFunc;
            llvm::Function* m_constructDblFunc;;
            llvm::Function* m_copyPrimitiveFunc;
            llvm::Function* m_destroyPrimitiveFunc;
            llvm::Function* m_setElemFunc;
            llvm::Function* m_isEqualFunc;
            llvm::Function* m_isNotEqualFunc;
            llvm::Function* m_toBoolFunc;

            llvm::Function* m_addFunc; // Function for adding primitives to each other.
            llvm::Function* m_subFunc;
            llvm::Function* m_mulFunc;
            llvm::Function* m_divFunc;
            llvm::Function* m_lessThanFunc;
            llvm::Function* m_greaterThanFunc;
            llvm::Function* m_lessThanOrEqualToFunc;
            llvm::Function* m_greaterThanOrEqualToFunc;
            llvm::Function* m_andFunc;
            llvm::Function* m_orFunc;
            llvm::Function* m_notFunc;
            llvm::Function* m_negativeFunc;

            llvm::Function* loadStdFunc(const std::string& name);

        };
    }
}
