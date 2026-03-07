# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -I./src
LDFLAGS = 

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
REGEX_SOURCES = $(SRC_DIR)/regex/NFA.cpp \
                $(SRC_DIR)/regex/DFA.cpp \
                $(SRC_DIR)/regex/RegexParser.cpp

LEXER_SOURCES = $(SRC_DIR)/lexer/Token.cpp \
                $(SRC_DIR)/lexer/Lexer.cpp

PARSER_SOURCES = $(SRC_DIR)/parser/AST.cpp \
                 $(SRC_DIR)/parser/Parser.cpp

IR_SOURCES = $(SRC_DIR)/ir/IR.cpp \
             $(SRC_DIR)/ir/IRGenerator.cpp \
             $(SRC_DIR)/ir/IROptimizer.cpp

CODEGEN_SOURCES = $(SRC_DIR)/codegen/CodeGenerator.cpp

MAIN_SOURCE = $(SRC_DIR)/main.cpp

# All sources
SOURCES = $(MAIN_SOURCE) $(REGEX_SOURCES) $(LEXER_SOURCES) $(PARSER_SOURCES) $(IR_SOURCES) $(CODEGEN_SOURCES)

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable
TARGET = $(BIN_DIR)/cs2cpp

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(BUILD_DIR)/regex
	@mkdir -p $(BUILD_DIR)/lexer
	@mkdir -p $(BUILD_DIR)/parser
	@mkdir -p $(BUILD_DIR)/ir
	@mkdir -p $(BUILD_DIR)/codegen
	@mkdir -p $(BIN_DIR)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build successful! Executable: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Compile individual tests
compile-tests: all
	@echo "Compiling test suite..."
	@mkdir -p tests/bin
	@$(CXX) $(CXXFLAGS) -o tests/bin/test_ir tests/test_ir.cpp \
		$(REGEX_SOURCES) $(LEXER_SOURCES) $(PARSER_SOURCES) $(IR_SOURCES)
	@echo "Tests compiled successfully"

# Run tests
test: compile-tests
	@echo "Running tests..."
	@cd tests && bash run_tests.sh

# Install
install: all
	cp $(TARGET) /usr/local/bin/
	@echo "Installed to /usr/local/bin/cs2cpp"

# Uninstall
uninstall:
	rm -f /usr/local/bin/cs2cpp
	@echo "Uninstalled cs2cpp"

# Help
help:
	@echo "C# to C++ Compiler - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the compiler (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  test      - Run test suite"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help message"

.PHONY: all directories clean test install uninstall help
