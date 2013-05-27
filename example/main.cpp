#include "../src/ModuleLoader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Module.h"

static llvm::cl::list<std::string>
InputFilenames(llvm::cl::Positional, llvm::cl::desc("<input preprocessed files>"));

using namespace laser;

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv, "main");
  
  ModuleLoader mloader(getGlobalContext());
  Module *module = mloader.loadInputFiles(InputFilenames);
  if (!module)
    mloader.dumpErrorMessages();
  else
    module->dump();
  return 0;
}
