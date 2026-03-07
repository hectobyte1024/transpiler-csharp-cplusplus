#!/bin/bash

# C# to C++ Compiler Test Suite

echo "========================================="
echo "C# to C++ Compiler - Test Suite"
echo "========================================="
echo ""

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counter for tests
PASSED=0
FAILED=0

# Function to run a test
run_test() {
    local test_name=$1
    local test_binary=$2
    
    echo -n "Running $test_name... "
    
    if [ -f "$test_binary" ]; then
        if ./"$test_binary" > /dev/null 2>&1; then
            echo -e "${GREEN}PASSED${NC}"
            ((PASSED++))
        else
            echo -e "${RED}FAILED${NC}"
            ((FAILED++))
        fi
    else
        echo -e "${YELLOW}SKIPPED${NC} (binary not found)"
    fi
}

# Compile tests if needed
compile_tests() {
    echo "Compiling tests..."
    echo ""
    
    if [ -d "../build" ]; then
        cd ../build
        make > /dev/null 2>&1
        cd ../tests
    else
        echo -e "${YELLOW}Warning: Build directory not found. Run cmake first.${NC}"
        echo ""
    fi
}

# Check if compiler exists
if [ ! -f "../bin/cs2cpp" ] && [ ! -f "../build/cs2cpp" ]; then
    echo -e "${YELLOW}Compiler not found. Building...${NC}"
    echo ""
    cd ..
    make
    cd tests
    echo ""
fi

# Compile tests
compile_tests

# Run unit tests
echo "=== Unit Tests ==="
echo ""

run_test "Regex Engine" "../build/tests/test_regex"
run_test "Lexer" "../build/tests/test_lexer"
run_test "Parser" "../build/tests/test_parser"
run_test "IR & Optimization" "bin/test_ir"

echo ""
echo "=== Integration Tests ==="
echo ""

# Find the compiler
COMPILER="../bin/cs2cpp"
if [ ! -f "$COMPILER" ]; then
    COMPILER="../build/cs2cpp"
fi

if [ ! -f "$COMPILER" ]; then
    COMPILER="../cs2cpp"
fi

# Integration test: Compile example programs
if [ -f "$COMPILER" ]; then
    for example in ../examples/*.cs; do
        if [ -f "$example" ]; then
            basename_ex=$(basename "$example" .cs)
            echo -n "Compiling $basename_ex.cs... "
            
            if "$COMPILER" "$example" "output_$basename_ex.cpp" > /dev/null 2>&1; then
                if [ -f "output_$basename_ex.cpp" ]; then
                    echo -e "${GREEN}PASSED${NC}"
                    ((PASSED++))
                    rm -f "output_$basename_ex.cpp"
                else
                    echo -e "${RED}FAILED${NC} (no output)"
                    ((FAILED++))
                fi
            else
                echo -e "${RED}FAILED${NC}"
                ((FAILED++))
            fi
        fi
    done
else
    echo -e "${RED}Compiler binary not found!${NC}"
fi

# Summary
echo ""
echo "========================================="
echo "Test Results"
echo "========================================="
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}✗ Some tests failed${NC}"
    exit 1
fi
