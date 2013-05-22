SOURCES = test1.c test2.c
OBJECTS = $(SOURCES:.c=.s) $(SOURCES:.c=.o)

all: $(OBJECTS)
%.s: %.c
	clang -emit-llvm -S $<
%.o: %.c
	clang -emit-llvm -c $<

.PHONY: clean
clean:
	rm -f *.s *.o