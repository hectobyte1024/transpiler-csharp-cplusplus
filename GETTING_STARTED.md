# Getting Started with the C# to C++ Compiler

## Quick Start

### 1. Build the Compiler

```bash
# Using Make (recommended)
make

# Or using CMake
mkdir build && cd build
cmake ..
make
cd ..
```

### 2. Run Your First Compilation

```bash
# Compile a C# file to C++
./bin/cs2cpp examples/hello.cs output.cpp

# This generates TWO files:
# - output.h (header file)
# - output.cpp (source file)

# View the generated C++ code
cat output.h
cat output.cpp
```

### 3. More Examples

```bash
# Interface and inheritance
./bin/cs2cpp examples/interface.cs shape.cpp

# Generic classes (templates)
./bin/cs2cpp examples/generics.cs box.cpp

# Exception handling
./bin/cs2cpp examples/exceptions.cs error.cpp
```

### 4. Test the Compiler

```bash
# Run all tests
cd tests
./run_tests.sh
```

## Example Usage

### Example 1: Simple Program

**Input (input.cs):**
```csharp
class MyClass
{
    public static void Main()
    {
        int x = 10;
        int y = 20;
        int sum = x + y;
    }
}
```

**Compile:**
```bash
./bin/cs2cpp input.cs output.cpp
```

**Output (output.cpp):**
```cpp
#include <iostream>
#include <string>
#include <memory>

class MyClass
{
public:
    static void Main()
    {
        int x = 10;
        int y = 20;
        int sum = (x + y);
    }

};
```

### Example 2: Control Flow

**Input:**
```csharp
class Program
{
    public static void Test()
    {
        int n = 10;
        
        if (n > 5)
        {
            n = n + 1;
        }
        else
        {
            n = n - 1;
        }
    }
}
```

**Generated C++:**
```cpp
class Program
{
public:
    static void Test()
    {
        int n = 10;
        if ((n > 5))
        {
            (n = (n + 1));
        }
        else
        {
            (n = (n - 1));
        }
    }

};
```

## Command-Line Options

```bash
./bin/cs2cpp <input.cs> [output.cpp]
```

- `input.cs` - Input C# source file (required)
- `output.cpp` - Output C++ file (optional, defaults to `output.cpp`)

## Project Structure

```
.
├── bin/                    # Compiled executables
│   └── cs2cpp             # Main compiler executable
├── build/                  # Build artifacts
├── examples/               # Example C# programs
│   ├── hello.cs
│   ├── calculator.cs
│   └── loops.cs
├── src/                    # Source code
│   ├── regex/             # Regular expression engine
│   ├── lexer/             # Lexical analyzer
│   ├── parser/            # Syntax analyzer
│   ├── codegen/           # Code generator
│   └── main.cpp           # Compiler driver
├── tests/                  # Test suite
├── CMakeLists.txt         # CMake configuration
├── Makefile               # Make configuration
├── README.md              # Main documentation
└── THEORY.md              # Theoretical foundation
```

## Understanding the Components

### Regex Engine (`src/regex/`)

Implements regular expressions from scratch:
- **NFA.cpp**: Thompson Construction algorithm
- **DFA.cpp**: Subset Construction (NFA→DFA conversion)
- **RegexParser.cpp**: Pattern parsing and matching

### Lexer (`src/lexer/`)

Tokenizes C# source code:
- **Token.h/cpp**: Token definitions
- **Lexer.h/cpp**: Finite automata-based lexical analysis

### Parser (`src/parser/`)

Builds Abstract Syntax Tree:
- **AST.h/cpp**: AST node definitions
- **Parser.h/cpp**: Recursive descent parser

### Code Generator (`src/codegen/`)

Generates C++ code:
- **CodeGenerator.h/cpp**: AST visitor that outputs C++

## Supported C# Features

✅ **Supported:**
- Classes
- Methods (static and instance)
- Variables and assignments
- Arithmetic expressions (+, -, *, /, %)
- Comparison operators (<, <=, >, >=, ==, !=)
- Logical operators (&&, ||, !)
- Control flow (if/else, while, for)
- Return statements
- Access modifiers (public, private, protected)
- Basic types (int, float, double, bool, char, string)

❌ **Not Supported (yet):**
- Generics
- LINQ
- Properties
- Events
- Lambda expressions
- async/await
- Inheritance
- Interfaces
- Exception handling

## Troubleshooting

### Build Errors

**Problem:** `g++: command not found`
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# Fedora
sudo dnf install gcc-c++

# macOS
xcode-select --install
```

**Problem:** `C++17 not supported`
- Update your compiler to GCC 7+ or Clang 5+

### Runtime Errors

**Problem:** `Error: Could not open file`
- Check that the input file exists
- Use absolute or relative path correctly

**Problem:** `Error: Expected expression at line X`
- The C# syntax may not be supported yet
- Check the supported features list above
- Simplify the code to use only supported constructs

## Next Steps

1. **Explore Examples**: Check `examples/` directory for more C# programs
2. **Run Tests**: Execute `tests/run_tests.sh` to verify everything works
3. **Read Theory**: Study `THEORY.md` to understand the implementation
4. **Extend the Compiler**: Add new features by modifying the source files

## Learning Resources

- **Dragon Book**: "Compilers: Principles, Techniques, and Tools"
- **Automata Theory**: Understand finite automata and regular expressions
- **C++ Programming**: Modern C++17 features used in this project
- **Compiler Design**: Learn about lexical analysis, parsing, and code generation

## Contributing

This is an educational project. Feel free to:
- Add new language features
- Improve error messages
- Optimize the code generator
- Add more test cases
- Enhance documentation

## Support

For questions or issues:
1. Check the documentation in `README.md` and `THEORY.md`
2. Review the source code comments
3. Examine the test files in `tests/`
4. Run with verbose mode (modify main.cpp to set `verbose = true`)

---

**Happy Compiling!** 🚀
