//===-- ModuleLoader - Class to load llvm IR/BCs as a module--*- C++ -*-===//

#include "llvm/Support/CommandLine.h"
#include <string>
#include <list>

namespace llvm {

class Module;
class LLVMContext;
class raw_ostream;

}

using namespace llvm;

namespace xuzb {

class ModuleLoader {
  typedef std::list<std::string> ErrorMsgList;
  LLVMContext &llvmCtx;
  ErrorMsgList errorMsgs;
  
public:
  ModuleLoader(LLVMContext &ctx) : llvmCtx(ctx) {}

  Module *loadInputFiles(const std::list<std::string> &inputFilenames);
  Module *loadInputFiles(const cl::list<std::string> &inputFilenames);
  Module *loadInputFiles(int nFiles, const char **inputFilenames);

  Module *loadBitcodeFiles(const std::list<std::string> &inputFilenames);
  Module *loadBitcodeFiles(const cl::list<std::string> &inputFilenames);
  Module *loadBitcodeFiles(int nFiles, const char **inputFilenames);

  Module *loadAssemblyFiles(const std::list<std::string> &inputFilenames);
  Module *loadAssemblyFiles(const cl::list<std::string> &inputFilenames);
  Module *loadAssemblyFiles(int nFiles, const char **inputFilenames);
  
  Module *loadBitcodeFile(const std::string &filename);
  Module *loadBitcodeFile(const char *filename);
  
  Module *loadAssemblyFile(const std::string &filename);
  Module *loadAssemblyFile(const char *filename);

  bool mergeModules(Module *dest, Module *src);

public:
  // Method for Error Diagnostic
  void printErrorMessages(raw_ostream &os) const;
  void dumpErrorMessages() const;

private:
  void addErrorMessage(const std::string &msg) {
    errorMsgs.push_back(msg);
  }

  void convertToFilenameList(const cl::list<std::string> &src,
                             std::list<std::string> &dst);

  void convertToFilenameList(int nFiles, const char **src,
                             std::list<std::string> &dst);
};

}
