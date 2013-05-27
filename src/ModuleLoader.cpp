#include "ModuleLoader.h"
#include "llvm/Linker.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Assembly/Parser.h"
#include <string>

using namespace laser;

Module *ModuleLoader::loadInputFiles(const std::list<std::string> &inputFilenames) {
  OwningPtr<Module> mainModule;
  for (std::list<std::string>::const_iterator i = inputFilenames.begin(),
                                              e = inputFilenames.end(); i != e; ++i) {
    StringRef filename(*i);

    // Load Module from file
    Module *module = NULL;
    if (filename.endswith(".bc") || filename.endswith(".o")) 
      module = loadBitcodeFile(filename);
    else if (filename.endswith(".s"))
      module = loadAssemblyFile(filename);
    else {
      std::string errMsg("Unknown file type ");
      errMsg += filename;
      addErrorMessage(errMsg);
      return NULL;
    }
    
    if (!module)
      return NULL;
    
    if (!mainModule.get())
      mainModule.reset(module);
    else {
      // Merge module to mainModule and release module.
      if (!mergeModules(mainModule.get(), module))
        return NULL;
    }
  }
  return mainModule.take();
}

Module *ModuleLoader::loadInputFiles(const cl::list<std::string> &inputFilenames) {
  std::list<std::string> filenames;
  convertToFilenameList(inputFilenames, filenames);
  return loadInputFiles(filenames);
}

Module *ModuleLoader::loadInputFiles(int nFiles, const char **inputFilenames) {
  std::list<std::string> filenames;
  convertToFilenameList(nFiles, inputFilenames, filenames);
  return loadInputFiles(filenames);
}

Module *ModuleLoader::loadBitcodeFiles(const std::list<std::string> &inputFilenames) {
  OwningPtr<Module> mainModule;
  for (std::list<std::string>::const_iterator i = inputFilenames.begin(),
                                              e = inputFilenames.end(); i != e; ++i) {
    StringRef filename(*i);

    Module *module = loadBitcodeFile(filename);

    if (!module)
      return NULL;
    
    if (!mainModule.get())
      mainModule.reset(module);
    else {
      if (!mergeModules(mainModule.get(), module))
        return NULL;
    }
  }
  return mainModule.take();
}

Module *ModuleLoader::loadBitcodeFiles(const cl::list<std::string> &inputFilenames) {
  std::list<std::string> filenames;
  convertToFilenameList(inputFilenames, filenames);
  return loadBitcodeFiles(filenames);
}

Module *ModuleLoader::loadBitcodeFiles(int nFiles, const char **inputFilenames) {
  std::list<std::string> filenames;
  convertToFilenameList(nFiles, inputFilenames, filenames);
  return loadBitcodeFiles(filenames);
}

Module *ModuleLoader::loadAssemblyFiles(const std::list<std::string> &inputFilenames) {
  OwningPtr<Module> mainModule;
  for (std::list<std::string>::const_iterator i = inputFilenames.begin(),
                                              e = inputFilenames.end(); i != e; ++i) {
    StringRef filename(*i);

    Module *module = loadAssemblyFile(filename);

    if (!module)
      return NULL;
    
    if (!mainModule.get())
      mainModule.reset(module);
    else {
      if (!mergeModules(mainModule.get(), module))
        return NULL;
    }
  }
  return mainModule.take();
}

Module *ModuleLoader::loadAssemblyFiles(const cl::list<std::string> &inputFilenames) {
  std::list<std::string> filenames;
  convertToFilenameList(inputFilenames, filenames);
  return loadAssemblyFiles(filenames);
}

Module *ModuleLoader::loadAssemblyFiles(int nFiles, const char **inputFilenames) {
  std::list<std::string> filenames;
  convertToFilenameList(nFiles, inputFilenames, filenames);
  return loadAssemblyFiles(filenames);
}

Module *ModuleLoader::loadBitcodeFile(const std::string &filename) {
  return loadBitcodeFile(filename.c_str());
}

Module *ModuleLoader::loadBitcodeFile(const char *filename) {
  // Load file content
  llvm::OwningPtr<MemoryBuffer> BufferPtr;
  error_code ec = MemoryBuffer::getFileOrSTDIN(filename, BufferPtr);
  if (ec) {
    addErrorMessage(ec.message());
    return NULL;
  }

  // Parse as Bitcode
  std::string errMsg;
  Module *module = ParseBitcodeFile(BufferPtr.get(), llvmCtx, &errMsg);
  if (module) {
    // Verify the generated module.
    if (module->MaterializeAllPermanently(&errMsg)) {
      delete module;
      module = NULL;
    }
  }
  
  if (module == NULL) {
    addErrorMessage(errMsg);
  }

  return module;
}

Module *ModuleLoader::loadAssemblyFile(const std::string &filename) {
  // Load from assembly file
  SMDiagnostic diag;
  Module *module = ParseAssemblyFile(filename, diag, llvmCtx);
  if (!module) {
    addErrorMessage(diag.getMessage());
    return NULL;
  }

  // Verify the generated module.
  std::string errMsg;
  if (module->MaterializeAllPermanently(&errMsg)) {
    delete module;
    addErrorMessage(errMsg);
    return NULL;
  }

  return module;
}


Module *ModuleLoader::loadAssemblyFile(const char *filename) {
  loadAssemblyFile(std::string(filename));
}

    
bool ModuleLoader::mergeModules(Module *dst, Module *src) {
  std::string errMsg;
  if (Linker::LinkModules(dst, src, Linker::DestroySource, &errMsg)) {
    addErrorMessage(errMsg);
    return false;
  }
  
  return true;
}

void ModuleLoader::printErrorMessages(raw_ostream &os) const {
  os << "//******Error Messages******//\n\n";
  for (ErrorMsgList::const_iterator i = errorMsgs.begin(),
                                    e = errorMsgs.end(); i != e; ++i) {
    os << *i << "\n";
  }
  os << "\n//**************************//\n";
}

void ModuleLoader::dumpErrorMessages() const {
  printErrorMessages(errs());
}


void ModuleLoader::convertToFilenameList(const cl::list<std::string> &src,
                                         std::list<std::string> &dst) {
  for (int i = 0; i < src.size(); ++i) {
    dst.push_back(src[i]);
  }
}

void ModuleLoader::convertToFilenameList(int nFiles, const char **src,
                                         std::list<std::string> &dst) {
  for (int i = 0; i < nFiles; ++i) {
    dst.push_back(std::string(src[i]));
  }
}
