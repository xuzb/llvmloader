CXX := g++
RTTIFLAG := -fno-rtti
LLVM_BIN_DIR := $(HOME)/llvm-build/Debug+Asserts/bin
CXXFLAGS := $(shell $(LLVM_BIN_DIR)/llvm-config --cxxflags) $(RTTIFLAG)
SOURCES = ModuleLoader.cpp
OBJECTS = $(SOURCES:.cpp=.o)
LIB = libModuleLoader.a

all: $(OBJECTS) $(LIB)
%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
$(LIB):
	ar rcs $(LIB) $(OBJECTS)

clean:
	rm -f *.o *.a *.so
