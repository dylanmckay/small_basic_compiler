# Makefile

CXX  := g++
LD   := g++
RM   := rm -f

BIN   := small

CXXFLAGS  := --std=c++11 -g -Wall -Werror -std=c++11
LDFLAGS   := -L/usr/lib/llvm
LIBS      := -pthread -ldl $(shell llvm-config --libs)

CXX_SOURCE_FILES := $(shell find source/src -name "*.cpp")
CXX_OBJECT_FILES := $(patsubst %.cpp,%.o,$(CXX_SOURCE_FILES))

DEPS := $(CXX_OBJECT_FILES:.o=.d)

all: $(BIN)

clean:
	$(RM) $(CXX_OBJECT_FILES) $(DEPS) $(BIN)

rebuild: clean all

report:
	cloc $(shell find -name "*.cpp") $(CXX_SOURCE_FILES)

$(BIN): $(CXX_OBJECT_FILES)
	$(LD) $(CXX_OBJECT_FILES) $(LDFLAGS) $(LIBS) -o $@

%.o: %.cpp
	$(CXX) -c -MMD -MP $< -o $@ $(CXXFLAGS)

-include $(DEPS)

