# Small basic stdlib makefile.


CC  := clang -S -emit-llvm
LD  := llvm-link
RM  := rm -f

BIN := stdlib.bc

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.cpp,%.ll,$(SOURCES))

all: $(BIN)

clean:
	$(RM) $(OBJECTS) $(BIN)
	
rebuild: clean all

$(BIN): $(OBJECTS)
	$(LD) $(OBJECTS) -o $@

%.ll: %.cpp
	$(CC) $< -o $@

%.ll: %.c
	$(CC) $< -o $@
	
test: test.cpp $(OBJECTS)
#g++ -g -std=c++11 $(SOURCES) test.cpp -o test
	$(CC) test.cpp -o test.ll
	clang $(OBJECTS) test.ll -o test
