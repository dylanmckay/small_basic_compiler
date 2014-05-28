#pragma once

namespace epsilon
{
    namespace codegen
    {
        class LLVMGenerator;

        class StandardLibrary
        {

        public:

            static void AddToGenerator(LLVMGenerator& gen);

            static void AddTextWindow(LLVMGenerator& gen);

        };
    }
}
