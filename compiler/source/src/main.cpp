

#include "../include/epsilon/io/StringReader.hpp"
#include "../include/epsilon/io/FileStream.hpp"
#include "../include/epsilon/io/StreamTextReader.hpp"
#include "../include/epsilon/parser/Tokenizer.hpp"
#include "../include/epsilon/parser/Parser.hpp"
#include "../include/epsilon/codegen/LLVMGenerator.hpp"
#include "../include/epsilon/codegen/ClangHelper.hpp"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/IRReader.h>
#include <llvm/LLVMContext.h>
#include <llvm/Support/TargetRegistry.h>


// MCPU
#include "llvm/ADT/Triple.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/DataLayout.h"
#include "llvm/Module.h"
#include "llvm/Support/IRReader.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "llvm/Target/TargetMachine.h"

#include <iostream>

using namespace epsilon;
using namespace parser;
using namespace expressions;
using namespace statements;
using namespace io;
using namespace codegen;

using namespace std;
using namespace llvm;


/*!
 * Generates an LLVM module.
 */
llvm::Module* GenerateModule(ITextReader& reader);

void OutputIR(llvm::Module& mod, raw_ostream& stream);
void OutputBitcode(llvm::Module& mod, raw_ostream& stream);
void OutputObjectFile(llvm::Module& mod, raw_ostream& stream);

void RunModule(llvm::Module& mod);

enum class OutputType
{
    IR,
    Bitcode,
    NativeObject
};

enum class Action
{
    Compile,
    Run,
    DoNothing
};

void DoCompile(int argc, char* argv[])
{
    OutputType outType = OutputType::IR;
    ITextReader* sourceReader = nullptr;
    raw_ostream* outStream = nullptr;
    bool deleteOut = true; // Wether outStream should be deleted.
    Action action = Action::Compile;

    // Loop through the arguments.
    for(unsigned int i=1; i<(unsigned int)argc; i++)
    {
        // Print help.
        if(strcmp(argv[i], "--help") == 0)
        {
            cout << "Compiler: v0.9" << endl << endl;

            cout << argv[0] << " [options] <source_file>" << endl;

            cout << "Options:" << endl;

            action = Action::DoNothing;

            break;
        }
        else if(strcmp(argv[i], "-run") == 0) // Check for '-run; flag.
        {
            action = Action::Run;
        }
        else if(strcmp(argv[i], "-o") == 0) // Check for '-o' flag.
        {
            i++;

            if(i >= (unsigned int)argc)
                throw std::runtime_error("expected file name after '-o'");

            // raw_fd_ostream supports '-' as the filename, in which case
            // it will open stdout and close it afterwards; we don't want that.
            if(strcmp(argv[i], "-") == 0)
            {
                throw std::runtime_error("invalid filename character");
            }

            std::string err;

            // Open the file.
            outStream = new raw_fd_ostream(argv[i], err);

            // Check if the file was open failed.
            if(((raw_fd_ostream*)outStream)->has_error())
            {
                throw std::runtime_error(err);
            }
        }
        else if(strcmp(argv[i], "--filetype") == 0) // Check for --filetype=[type] flag.
        {
            i++;

            if(i >= (unsigned int) argc)
                throw std::runtime_error("expected file type");

            if(strcmp(argv[i], "llvm-ir") == 0) // LLVM IR output.
            {
                outType = OutputType::IR;
            }
            else if(strcmp(argv[i], "llvm-bc") == 0) // LLVM BC output.
            {
                outType = OutputType::Bitcode;
            }
            else if(strcmp(argv[i], "obj") == 0) // Native object file.
            {
                outType = OutputType::NativeObject;
            }
            else // Invalid file type.
            {
                throw std::runtime_error("invalid file type, run with '--help' for options");
            }
        }
        else // The argument must be a source file.
        {
            // Open the file and give it to a stream text reader, deleting the stream when the reader is deleted.
            sourceReader = new StreamTextReader(new FileStream(argv[i], FileStream::FileMode::Open), true);

            deleteOut = false; // Don't delete stdout (haha..)
        }
    }


    if(action == Action::Compile)
    {
        // Check if an output stream was assigned.
        if(!outStream)
            outStream = &outs(); // It wasn't, use stdout.

        if(!sourceReader)
            throw std::runtime_error("no source file specified");

        Module* module = GenerateModule(*sourceReader);

        switch(outType)
        {
            case OutputType::IR:
            {
                OutputIR(*module, *outStream);

                break;
            }
            case OutputType::Bitcode:
            {
                OutputBitcode(*module, *outStream);

                break;
            }
            case OutputType::NativeObject:
            {
                OutputObjectFile(*module, *outStream);

                break;
            }
        }

        delete module;
    }
    else if(action == Action::Run)
    {
        // Check if an output stream was assigned.
        if(!outStream)
            outStream = &outs(); // It wasn't, use stdout.

        if(!sourceReader)
            throw std::runtime_error("no source file specified");

        Module* module = GenerateModule(*sourceReader);

        RunModule(*module);

        delete module;
    }

    delete sourceReader;

    if(deleteOut)
        delete outStream;


}

int main(int argc, char* argv[])
{
    InitializeNativeTarget();

    try
    {
        DoCompile(argc, argv);
    }
    catch(std::runtime_error& e)
    {
        cerr << "Error: " << e.what() << endl;
    }


    return 0;
}


llvm::Module* GenerateModule(ITextReader& reader)
{
    LLVMGenerator gen;
    Parser parser(reader);

    Statement* statement;

    while((statement = parser.parseNextStatement()) != nullptr)
    {
        gen.processStatement(statement);
        statement->drop();
    }


    return gen.generateCode();
}

void OutputIR(llvm::Module& mod, raw_ostream& stream)
{
    mod.print(stream, nullptr);
}

void OutputBitcode(llvm::Module& mod, raw_ostream& stream)
{
    WriteBitcodeToFile(&mod, stream);
}

void OutputObjectFile(llvm::Module& mod, raw_ostream& stream)
{
    InitializeAllTargets();

    Triple targetTriple("x86_64", "pc", "linux", "gnu");

    std::string err;

    const Target* target = TargetRegistry::lookupTarget(targetTriple.getTriple(), err);

    if(!target)
        throw std::runtime_error(err);

    TargetOptions targetOptions;

    TargetMachine* machine = target->createTargetMachine(targetTriple.getTriple(), MCPU, "", targetOptions);

    if(!machine)
        throw std::runtime_error("could not create target machine");

    //machine->setMCRelaxAll(true);

    PassManager PM;

    //TargetLibraryInfo *TLI = new TargetLibraryInfo(targetTriple);


    //PM.add(TLI);
    //machine->addAnalysisPasses(PM);
    //PM.add(new DataLayout(&mod));

    formatted_raw_ostream actualStream(stream);

    machine->addPassesToEmitFile(PM, actualStream, TargetMachine::CGFT_ObjectFile);

    PM.run(mod);
}

void RunModule(llvm::Module& mod)
{
    std::string err;

    ExecutionEngine* engine = EngineBuilder(&mod).setErrorStr(&err).create();

    if(!engine)
        throw std::runtime_error(err);

    typedef void (*mainfunc_t)();

    mainfunc_t func = (mainfunc_t) engine->getPointerToFunction(mod.getFunction("SBMain"));

    func();
}
