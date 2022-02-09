MKDIR    := mkdir -p
RMDIR    := rm -rf
CXX      := clang++
TARGET   := ./target
OBJ      := ./bin
LIBOBJ   := ./bin/lib
SRC      := ./src
LIB      := ./libs
SRCS     := $(shell find $(SRC) -name '*.cpp')
LIBS     := $(shell find $(LIB) \( -name '*.cpp' -o -name '*.cc' \))
OBJS     := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))
LIBOBJS  := $(filter %.o, $(LIBS:$(LIB)/%.cpp=$(LIBOBJ)/%.o) $(LIBS:$(LIB)/%.cc=$(LIBOBJ)/%.o))
EXE      := $(TARGET)/SudokuSolver
CXXFLAGS := -I$(SRC) -I$(LIB) -std=c++20 -stdlib=libc++ -Wall -Wextra -Wpedantic -MD -MP
LDLIBS   := -lm
LDFLAGS  :=

DEBUG    ?= 0

ifeq ($(DEBUG),1)
    CXXFLAGS += -g -O0 -DDEBUG
else
    CXXFLAGS += -O2
endif

.PHONY: all run clean printstuff

all: $(EXE)

$(EXE): $(OBJS) $(LIBOBJS) | $(TARGET)
	@echo ""
	@echo "Linking $(@F)..."
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	@echo "\nBuild Successful!"

$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	@echo "Compiling $<"
	@$(MKDIR) -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBOBJ)/%.o: $(LIB)/%.cc | $(OBJ)
	@echo "Compiling lib $<"
	@$(MKDIR) -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET) $(OBJ):
	@echo "Creating dir: $@"
	$(MKDIR) $@

run: $(EXE)
	$<

clean:
	$(RMDIR) $(OBJ) $(TARGET)

printstuff:
	@echo "sources: $(SRCS)"
	@echo "objects: $(OBJS)"
	@echo "libs: $(LIBS)"
	@echo "LIBobjects: $(LIBOBJS)"


# Add .d to Make's recognized suffixes.
SUFFIXES += .d

#We don't need to clean up when we're making these targets
NODEPS:=clean
#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst $(SRC)/%.cpp,$(OBJ)/%.d,$(SRCS))

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    #Chances are, these files don't exist.  GMake will create them and
    #clean up automatically afterwards
    -include $(DEPFILES)
endif
