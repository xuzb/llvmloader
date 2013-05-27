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
$(EXE): ../src/libModuleLoader.a
	$(CXX) -o $(EXE) $(OBJECTS) -L../src/ -lModuleLoader $(LLVMLDFLAGS)

../src/libModuleLoader.a:
	$(MAKE) -C ../src/

clean:
	rm -f *.o *.so main
