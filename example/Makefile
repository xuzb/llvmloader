CXX := g++
RTTIFLAG := -fno-rtti
LLVM_BIN_DIR := $(HOME)/llvm-build/Debug+Asserts/bin
CXXFLAGS := $(shell $(LLVM_BIN_DIR)/llvm-config --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell $(LLVM_BIN_DIR)/llvm-config --libs) $(shell $(LLVM_BIN_DIR)/llvm-config --ldflags)

SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXE = main

all: $(OBJECTS) $(EXE)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) 
$(EXE): ModuleLoader
	$(CXX) -o $(EXE) $(OBJECTS) -L../src/ -lModuleLoader $(LLVMLDFLAGS)

.PHONY: ModuleLoader
ModuleLoader:
	$(MAKE) -C ../src/

clean:
	rm -f *.o *.so main
