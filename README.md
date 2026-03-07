# C# to C++ Source-to-Source Compiler
## 6-Month Comprehensive Compiler Development Project

A production-grade compiler that translates C# source code to C++ with a **manually constructed lexical analyzer** based on finite automata theory, a **custom regular expression engine** built from scratch, and **advanced optimization infrastructure**.

**Project Timeline**: 6 months (Months 1-2 complete, Months 3-6 planned)  
**Current Status**: Month 2 Complete - IR & Optimization Infrastructure ✅  
**Next Phase**: Month 3 - Semantic Analysis & Type System 📅

## Key Features

### 🔧 No External Dependencies
- **No std::regex** - Custom regex implementation
- **No Boost** - Pure C++ standard library
- **No external tools** - Everything built from scratch

### 🎯 Core Components

1. **Regular Expression Engine**
   - Thompson Construction for NFA generation
   - Subset Construction (Powerset Construction) for NFA to DFA conversion
   - DFA simulation for pattern matching
   - Support for: concatenation, alternation (|), Kleene star (*), plus (+), optional (?)

2. **Lexical Analyzer (Lexer)**
   - Manually constructed based on finite automata theory
   - Token recognition using DFA simulation
   - Supports C# keywords, identifiers, literals, operators, and delimiters
   - Line and column tracking for error reporting

3. **Syntax Analyzer (Parser)**
   - Recursive descent parser
   - Generates Abstract Syntax Tree (AST)
   - Supports C# language constructs: classes, methods, control flow, expressions
   - Error recovery with synchronization points
   - Multiple error collection without cascading

4. **Intermediate Representation (IR)**
   - Three-address code format inspired by LLVM
   - Basic block structure for control flow analysis
   - Instructions: arithmetic, load/store, branches, function calls
   - Separation between parsing and code generation

5. **Optimization Passes**
   - **Constant Folding**: Evaluate constant expressions at compile time
   - **Dead Code Elimination**: Remove unused temporaries and instructions
   - **Copy Propagation**: Replace redundant loads with direct values
   - **Common Subexpression Elimination**: Reuse previously computed results
   - Iterative fixed-point optimization

6. **Code Generator**
   - Visitor pattern for AST traversal
   - Generates equivalent C++ code with proper formatting
   - Type conversion from C# to C++
   - Preserves program semantics
   - Configurable pretty printing options

## Project Structure

```
compilador_de_c#_a_c++/
├── src/
│   ├── regex/              # Custom Regular Expression Engine
│   │   ├── NFA.h           # Non-deterministic Finite Automaton
│   │   ├── NFA.cpp         # Thompson Construction implementation
│   │   ├── DFA.h           # Deterministic Finite Automaton
│   │   ├── DFA.cpp         # Subset Construction & DFA simulation
│   │   ├── RegexParser.h   # Regex pattern parser
│   │   └── RegexParser.cpp # Regex to NFA conversion
│   │
│   ├── lexer/              # Lexical Analyzer
│   │   ├── Token.h         # Token definitions
│   │   ├── Token.cpp       # Token implementation
│   │   ├── Lexer.h         # Lexer interface
│   │   └── Lexer.cpp       # Lexer based on finite automata
│   │
│   ├── parser/             # Syntax Analyzer
│   │   ├── AST.h           # Abstract Syntax Tree nodes
│   │   ├── AST.cpp         # AST implementation
│   │   ├── Parser.h        # Parser interface
│   │   └── Parser.cpp      # Recursive descent parser
│   │
│   ├── ir/                 # Intermediate Representation
│   │   ├── IR.h            # IR data structures (Module, Function, BasicBlock, Instruction)
│   │   ├── IR.cpp          # IR implementation and toString methods
│   │   ├── IRGenerator.h   # AST to IR translation
│   │   ├── IRGenerator.cpp # Three-address code generation
│   │   ├── IROptimizer.h   # Optimization pass framework
│   │   └── IROptimizer.cpp # Optimization pass implementations
│   │
│   ├── codegen/            # Code Generator
│   │   ├── CodeGenerator.h # Code generator interface
│   │   └── CodeGenerator.cpp # C++ code generation and pretty printing
│   │
│   └── main.cpp            # Compiler driver
│
├── tests/                  # Test programs
├── examples/               # Example C# programs
├── CMakeLists.txt          # CMake build configuration
├── Makefile                # Alternative build system
└── README.md               # This file
```

## Building the Compiler

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+ (optional, for CMake build)

### Build with CMake

```bash
mkdir build
cd build
cmake ..
make
```

### Build with Make

```bash
make
```

### Build Manually

```bash
g++ -std=c++17 -Wall -Wextra -o cs2cpp \
    src/main.cpp \
    src/regex/NFA.cpp \
    src/regex/DFA.cpp \
    src/regex/RegexParser.cpp \
    src/lexer/Token.cpp \
    src/lexer/Lexer.cpp \
    src/parser/AST.cpp \
    src/parser/Parser.cpp \
    src/ir/IR.cpp \
    src/ir/IRGenerator.cpp \
    src/ir/IROptimizer.cpp \
    src/codegen/CodeGenerator.cpp
```

## Usage

```bash
./cs2cpp <input.cs> [output.cpp] [options]
```

### Command-Line Options

- `--emit-ir` - Generate intermediate representation instead of C++ code
- `--optimize` - Run optimization passes on the IR
- `--opt-level=N` - Set optimization level (1-3, default: 2)
- `--verbose` - Show detailed compilation information including tokens and IR

### Basic Usage

**Example:**
```bash
./cs2cpp examples/hello.cs output.cpp
```

This will generate two files:
- `output.h` - C++ header file with declarations
- `output.cpp` - C++ source file with implementations

### IR Generation

**Generate optimized IR:**
```bash
./cs2cpp examples/hello.cs --emit-ir --optimize
```

This will generate:
- `output.ir` - Three-address code intermediate representation

**View unoptimized IR:**
```bash
./cs2cpp examples/hello.cs --emit-ir --verbose
```

## Examples

### Basic Class Example
**Input** (`examples/simple_class.cs`):
```csharp
public class Circle
{
    private double radius;
    
    public void SetRadius(double r)
    {
        radius = r;
    }
    
    public double GetArea()
    {
        return 3.14159 * radius * radius;
    }
}
```

**Generated Header** (`output.h`):
```cpp
#ifndef GENERATED_H
#define GENERATED_H

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>

class Circle
{
    private:
        double radius;

public:
    void SetRadius(double r);
    double GetArea();
};

#endif // GENERATED_H
```

**Generated Source** (`output.cpp`):
```cpp
#include "generated.h"

void Circle::SetRadius(double r)
{
    radius = r;
}

double Circle::GetArea()
{
    return (3.14159 * (radius * radius));
}
```

### Interface Example
**Input** (`examples/interface.cs`):
```csharp
interface IShape
{
    double GetArea();
}

public class Circle : IShape
{
    private double radius;
    
    public double GetArea()
    {
        return 3.14159 * radius * radius;
    }
}
```

**Generated** (excerpt):
```cpp
// Interface: IShape
class IShape
{
    public:
        virtual double GetArea() = 0;
public:
    virtual ~IShape() = default;
};

class Circle : public IShape
{
    private:
        double radius;

public:
    double GetArea();
};
```

## Theoretical Foundation

### Thompson Construction
The NFA construction algorithm invented by Ken Thompson:
- Converts a regular expression to an NFA
- Each regex operator has a corresponding NFA fragment
- Fragments are combined to build the complete NFA

### Subset Construction
The powerset construction algorithm for NFA to DFA conversion:
- Each DFA state represents a set of NFA states
- Epsilon closure computation
- Transition table construction
- Produces a DFA that recognizes the same language

### DFA Simulation
Efficient pattern matching:
- O(n) time complexity for input of length n
- Deterministic state transitions
- Used for token recognition in the lexer

## Implementation Details

### Finite Automata Theory Applied

1. **State Management**: Each state has unique ID and acceptance status
2. **Transitions**: Character-based and epsilon (ε) transitions
3. **Epsilon Closure**: Computed using breadth-first search
4. **Move Function**: Computes reachable states on symbol
5. **Acceptance**: Input accepted if final state is accepting

### Token Recognition

The lexer uses a combination of:
- Manual state machines for common tokens (keywords, identifiers, numbers)
- DFA simulation for complex patterns
- Maximal munch principle for token boundary detection

## Supported Language Features

### Object-Oriented Programming
- Classes with fields and methods
- Access modifiers: `public`, `private`, `protected`
- Method modifiers: `static`, `virtual`, `abstract`, `override`, `sealed`
- Inheritance: single base class, multiple interfaces
- Interfaces → Compiled to pure virtual base classes

### Advanced Features
- **Exception Handling**: `try`, `catch`, `finally`, `throw`
- **Using Statements**: Compiled to RAII pattern in C++
- **Generics**: Basic template classes (e.g., `class Box<T>`)
- **Multi-File Output**: Automatic generation of header (.h) and source (.cpp) files

### Compiler Infrastructure
- **Intermediate Representation**: Three-address code for analysis and transformation
- **Optimization**: Multi-pass optimizer with dataflow analysis
- **Error Recovery**: Robust parsing with multiple error reporting
- **Pretty Printing**: Configurable code formatting options

### C# Constructs Supported
- Variable declarations with initializers
- Binary and unary expressions
- Literals: int, float, bool, string
- Function declarations with parameters
- Namespaces and using directives
- Control flow: if/else, while, for, return

## Limitations

This is an educational compiler demonstrating finite automata theory and compiler construction techniques. Current limitations:
- No constructor support yet
- No LINQ, async/await, or delegates
- Basic type system
- Limited semantic analysis
- IR-based optimization is a work-in-progress (not all optimizations connected to C++ generation yet)

## Extending the Compiler

To add support for new language features:

1. **Add tokens** in `src/lexer/Token.h`
2. **Update lexer** in `src/lexer/Lexer.cpp`
3. **Add AST nodes** in `src/parser/AST.h`
4. **Update parser** in `src/parser/Parser.cpp`
5. **Update IR generator** in `src/ir/IRGenerator.cpp` (if using IR pipeline)
6. **Update code generator** in `src/codegen/CodeGenerator.cpp`

To add optimization passes:

1. **Define pass** in `src/ir/IROptimizer.h`
2. **Implement analysis** in `src/ir/IROptimizer.cpp`
3. **Call from optimize()** method to integrate into pipeline

## Testing

### Run Complete Test Suite

The easiest way to run all tests:

```bash
make test
```

This will:
1. Compile the compiler
2. Compile test executables
3. Run unit tests (IR, optimization)
4. Run integration tests (all 10 example files)

### Run Tests Manually

```bash
cd tests
./run_tests.sh
```

### Test Coverage

**Unit Tests:**
- **IR Generation**: Variables, expressions, control flow (if/while/for), functions
- **Optimization**: Constant folding, dead code elimination, copy propagation, CSE
- **14 total IR tests** covering core compiler infrastructure

**Integration Tests:**
- All 10 example files compile successfully
- Examples include: inheritance, interfaces, exceptions, generics, loops

**Test Results:** 24/24 tests passing ✅

### Run Individual IR Tests

```bash
./tests/bin/test_ir
```

Output:
```
=== IR Generation Tests ===
✓ Variable declaration IR test passed
✓ Binary expression IR test passed
✓ If statement IR test passed
✓ While loop IR test passed
✓ For loop IR test passed
✓ Return statement IR test passed
✓ Multiple functions IR test passed
✓ Arithmetic operations IR test passed
✓ Comparison operations IR test passed

=== IR Optimization Tests ===
✓ Constant folding optimization test passed
✓ Dead code elimination test passed
✓ Copy propagation test passed
✓ Common subexpression elimination test passed
✓ Optimization iterations test passed

✅ All IR tests passed!
```

## 🗺️ Project Roadmap

This is a **6-month comprehensive compiler development project**. We're currently in Month 2 with advanced infrastructure complete.

**See [ROADMAP.md](ROADMAP.md) for the complete 6-month development plan:**
- ✅ **Month 1-2**: Core infrastructure, IR, optimization (COMPLETE)
- 📅 **Month 3**: Semantic analysis & type checking
- 📅 **Month 4**: Constructors, properties, operator overloading
- 📅 **Month 5**: SSA form, advanced optimizations, loop analysis
- 📅 **Month 6**: IR→C++ backend, register allocation, delegates, optional LLVM

**Next Milestone**: Semantic analysis framework (Month 3)
**See [MONTH3_QUICKSTART.md](MONTH3_QUICKSTART.md) for detailed Month 3 implementation guide**

### Projected Final Stats (Month 6)
- **~11,000 LOC** of production-quality compiler code
- **124+ tests** covering all components
- **25+ example programs** demonstrating C# to C++ translation
- **Complete optimization pipeline** with SSA form and advanced passes
- **Optional LLVM backend** for native code generation

## References

- Aho, Lam, Sethi, Ullman: "Compilers: Principles, Techniques, and Tools" (Dragon Book)
- Thompson, Ken: "Regular Expression Search Algorithm" (1968)
- Hopcroft, Ullman: "Introduction to Automata Theory, Languages, and Computation"
- Cooper & Torczon: "Engineering a Compiler" (2nd Edition)
- Appel, Andrew W.: "Modern Compiler Implementation in C" (1998)

## License

This project is for educational purposes. Feel free to use and modify for learning.

## Author

Compiler Automata Theory Implementation - 2026
