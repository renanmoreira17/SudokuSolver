MKDIR   := mkdir -p
RMDIR   := rm -rf
CXX     := clang++
TARGET  := ./target
OBJ     := ./bin
SRC     := ./src
SRCS    := $(shell find $(SRC) -name '*.cpp')
OBJS    := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))
EXE     := $(TARGET)/SudokuSolver
CXXFLAGS  := -I$(SRC) -std=c++20 -stdlib=libc++ -g -Wall -Wextra -Wpedantic
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