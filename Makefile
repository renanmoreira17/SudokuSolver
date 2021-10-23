MKDIR   := mkdir -p
RMDIR   := rm -rf
CXX     := clang++
TARGET  := ./target
OBJ     := ./bin
SRC     := ./src
SRCS    := $(shell find $(SRC) -name '*.cpp')
OBJS    := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))
EXE     := $(TARGET)/SudokuSolver
CXXFLAGS  := -I$(SRC) -std=c++20 -stdlib=libc++ -g -Wall -Wextra -Wpedantic -MD -MP
LDLIBS  := -lm
LDFLAGS :=

.PHONY: all run clean printstuff

all: $(EXE)

$(EXE): $(OBJS) | $(TARGET)
	@echo ""
	@echo "Linking $(@F)..."
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	@echo "Compiling $<"
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
