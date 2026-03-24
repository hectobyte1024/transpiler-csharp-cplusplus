# Complete Compilation Process: End-to-End Example

This document traces a complete C# program through all stages of the compiler, showing actual file references, data transformations, and the final C++ output.

---

## 📝 Input: Simple C# Program

**File**: `examples/simple.cs`

```csharp
class Program {
    public static void Main() {
        int x = 42;
        if (x > 0) {
            System.Console.WriteLine("Hello");
        }
    }
}
```

---

## 🔄 Stage 1: Frontend - Lexical Analysis

**Components**:
- File: `src/lexer/Lexer.h`, `src/lexer/Lexer.cpp`
- Engine: Custom regex DFA-based tokenizer
- Regex patterns: `src/lexer/Lexer.cpp` (lines 56-110 in `initializePatterns()`)

**Process**:
```
Input: "class Program { public static void Main() { ... }"
                    ↓
         [Regex engine - matchPattern()]
                    ↓
          Pattern matching via DFA
          Maximal-munch principle
                    ↓
      Token stream with line/column info
```

**Output Tokens** (first 20 of 20):

| # | Token Type | Lexeme | Line | Column |
|---|---|---|---|---|
| 1 | KW_CLASS | `class` | 1 | 1 |
| 2 | IDENTIFIER | `Program` | 1 | 7 |
| 3 | LBRACE | `{` | 1 | 15 |
| 4 | KW_PUBLIC | `public` | 2 | 5 |
| 5 | KW_STATIC | `static` | 2 | 12 |
| 6 | KW_VOID | `void` | 2 | 19 |
| 7 | IDENTIFIER | `Main` | 2 | 24 |
| 8 | LPAREN | `(` | 2 | 28 |
| 9 | RPAREN | `)` | 2 | 29 |
| 10 | LBRACE | `{` | 2 | 31 |
| 11 | KW_INT | `int` | 3 | 9 |
| 12 | IDENTIFIER | `x` | 3 | 13 |
| 13 | OP_ASSIGN | `=` | 3 | 15 |
| 14 | INTEGER_LITERAL | `42` | 3 | 17 |
| 15 | SEMICOLON | `;` | 3 | 19 |
| 16 | KW_IF | `if` | 4 | 9 |
| 17 | LPAREN | `(` | 4 | 12 |
| 18 | IDENTIFIER | `x` | 4 | 13 |
| 19 | OP_GREATER | `>` | 4 | 15 |
| 20 | INTEGER_LITERAL | `0` | 4 | 17 |

**Key Files Involved**:
- `src/lexer/Token.h` - Token structure definition
- `src/lexer/Token.cpp` - Token implementation
- `src/regex/RegexParser.h` - Regex pattern parser
- `src/regex/NFA.h`, `DFA.h` - Finite automata
- `src/lexer/Lexer.h` - Lexer API

---

## 🌳 Stage 2: Frontend - Syntactic Analysis

**Components**:
- File: `src/parser/Parser.h`, `src/parser/Parser.cpp`
- Method: Recursive descent parser
- Error recovery: Panic mode synchronization

**Process**:
```
Token stream
      ↓
[Recursive Descent Parser]
  - parseProgram()
  - parseClassDeclaration()
  - parseMemberDeclaration()
  - parseStatement()
  - parseExpression()
      ↓
Abstract Syntax Tree (AST)
```

**Output AST Structure**:

```
Program
├── ClassDeclaration "Program"
│   ├── accessModifier: PUBLIC
│   ├── members:
│   │   └── MethodDeclaration "Main"
│   │       ├── returnType: VOID
│   │       ├── accessModifier: PUBLIC
│   │       ├── isStatic: true
│   │       ├── parameters: [] (empty)
│   │       └── body: Block
│   │           ├── VariableDeclaration "x"
│   │           │   ├── type: INT
│   │           │   └── initializer: IntegerLiteral(42)
│   │           └── IfStatement
│   │               ├── condition: BinaryExpression
│   │               │   ├── left: Identifier("x")
│   │               │   ├── operator: GREATER
│   │               │   └── right: IntegerLiteral(0)
│   │               ├── thenBranch: Block
│   │               │   └── ExpressionStatement
│   │               │       └── MethodCall
│   │               │           ├── receiver: MemberAccess
│   │               │           │   ├── receiver: MemberAccess
│   │               │           │   │   ├── receiver: Identifier("System")
│   │               │           │   │   └── member: "Console"
│   │               │           │   └── member: "WriteLine"
│   │               │           └── arguments: ["Hello"]
│   │               └── elseBranch: null
└── (no namespace or using directives)
```

**Key Files Involved**:
- `src/parser/AST.h` - All AST node definitions
- `src/parser/AST.cpp` - AST implementations
- `src/parser/Parser.h` - Parser API
- `src/parser/Parser.cpp` - Parser implementation

---

## 🔍 Stage 3: Semantic Analysis (FUTURE - Month 3)

**This stage is not yet implemented but would add**:
- Symbol table population
- Type checking
- Scope validation
- Semantic error reporting

**Would use files**:
- `src/semantic/SymbolTable.h` (not yet created)
- `src/semantic/Type.h` (not yet created)
- `src/semantic/SemanticAnalyzer.h` (not yet created)

---

## 📊 Stage 4: IR Generation (Intermediate Representation)

**Components**:
- File: `src/ir/IR.h`, `src/ir/IR.cpp`
- File: `src/ir/IRGenerator.h`, `src/ir/IRGenerator.cpp`
- Format: Three-address code

**Process**:
```
AST (with visitor pattern)
  ↓
[IRGenerator visitor]
  - visit(ClassDeclaration)
  - visit(MethodDeclaration)
  - visit(VariableDeclaration)
  - visit(IfStatement)
  - visit(BinaryExpression)
  - visit(MethodCall)
  ↓
IR Instructions (3-addr code)
```

**Output IR Instructions** (simplified):

```ir
; Generated IR for simple.cs
; Class: Program

function void Program.Main()
  t1 = 42              ; IntegerLiteral: 42
  x = t1               ; VariableDeclaration: x = 42  (LINE 3)
  t2 = x               ; Load variable x
  t3 = 0               ; IntegerLiteral: 0
  cond = gt t2, t3     ; BinaryExpression: x > 0
  jz cond, L1          ; IfStatement: jump to L1 if false

  ; Then branch (x > 0 true)
  str = "Hello"        ; StringLiteral
  call System.Console.WriteLine(str)  ; MethodCall

L1:                    ; End of if statement
  ret void             ; Return from Main
end function Program.Main

; Optimization pass skipped for brevity
```

**Key Intermediate Representations**:
```cpp
struct IRInstruction {
    OpCode opcode;                      // ADD, SUB, MUL, LOAD, etc.
    std::string dest;                   // Destination: variable or temp
    std::string src1, src2;             // Source operands
    std::vector<std::string> metadata;  // Optional metadata
};

enum class OpCode {
    MOV,              // Move/assign
    ADD, SUB, MUL, DIV, MOD,  // Arithmetic
    LT, LE, GT, GE, EQ, NE,   // Comparison
    LOAD, STORE,      // Memory ops
    CALL,             // Function call
    RET,              // Return
    JZ, JMP,          // Branching
    LABEL             // Labels
};
```

**Key Files Involved**:
- `src/ir/IR.h` - IR instruction and basic block definitions
- `src/ir/IR.cpp` - IR implementation
- `src/ir/IRGenerator.h` - AST → IR conversion API
- `src/ir/IRGenerator.cpp` - AST visitor implementation

---

## ⚡ Stage 5: Optimization

**Components**:
- File: `src/ir/IROptimizer.h`, `src/ir/IROptimizer.cpp`
- Passes: Constant folding, dead code elimination, copy propagation, CSE

**Process**:
```
IR Instructions
      ↓
[Pass 1: Constant Folding]
  42 + 8 → 50
      ↓
[Pass 2: Dead Code Elimination]
  Remove unused assignments
      ↓
[Pass 3: Copy Propagation]
  x = y; z = x; → z = y;
      ↓
[Pass 4: Common Subexpression Elimination]
  a = x > 0; b = x > 0; → temp = x > 0; a = temp; b = temp;
      ↓
Optimized IR
```

**Key Files Involved**:
- `src/ir/IROptimizer.h` - Optimization API
- `src/ir/IROptimizer.cpp` - Optimization passes

---

## 💻 Stage 6: Code Generation

**Components**:
- File: `src/codegen/CodeGenerator.h`, `src/codegen/CodeGenerator.cpp`
- Output: C++ header (.h) and source (.cpp) files
- Pattern: AST visitor producing formatted code

**Process**:
```
AST + IR
    ↓
[CodeGenerator visitor]
  - visit(ClassDeclaration)
    ├─ Generate class declaration
    ├─ Generate member variables
    └─ Generate member methods
  - visit(MethodDeclaration)
    ├─ Generate function signature
    └─ Generate function body
  - visit(VariableDeclaration)
    ├─ Generate C++ type
    └─ Generate initialization
  - visit(IfStatement)
    ├─ Generate condition
    └─ Generate if/else blocks
    ↓
C++ source code (formatted)
```

**Key Files Involved**:
- `src/codegen/CodeGenerator.h` - Code generation API
- `src/codegen/CodeGenerator.cpp` - Code generation implementation
- `src/main.cpp` - Compiler driver and CLI orchestration

---

## 📄 Generated Output Files

### Output File 1: `output.h` (C++ Header)

```cpp
#ifndef GENERATED_PROGRAM_H
#define GENERATED_PROGRAM_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace Generated {

class Program {
public:
    static void Main();  // public static void Main()
};

} // namespace Generated

#endif // GENERATED_PROGRAM_H
```

### Output File 2: `output.cpp` (C++ Source)

```cpp
#include "output.h"

namespace Generated {

void Program::Main() {
    int x = 42;
    if (x > 0) {
        std::cout << "Hello" << std::endl;
    }
}

} // namespace Generated

int main() {
    Generated::Program::Main();
    return 0;
}
```

---

## 🔄 Complete Data Flow Diagram

```
Source File (simple.cs)
        │
        ├─→ [Lexer] 
        │   │  Files: Lexer.h, Lexer.cpp
        │   │  Uses: src/regex/* (DFA simulation)
        │   │  Output: Token stream
        │   ↓
        │  Token Stream (20 tokens)
        │   │
        │   ├─→ [Parser]
        │   │   │  Files: Parser.h, Parser.cpp, AST.h, AST.cpp
        │   │   │  Method: Recursive descent
        │   │   │  Output: AST
        │   │   ↓
        │   │  Abstract Syntax Tree
        │   │   │
        │   │   ├─┬─→ [IRGenerator]
        │   │   │ │   Files: IRGenerator.h, IRGenerator.cpp, IR.h, IR.cpp
        │   │   │ │   Pattern: AST visitor
        │   │   │ │   Output: IR instructions
        │   │   │ │   ↓
        │   │   │ │  IR (Three-address code)
        │   │   │ │   │
        │   │   │ │   ├─→ [IROptimizer]
        │   │   │ │   │   Files: IROptimizer.h, IROptimizer.cpp
        │   │   │ │   │   Passes: CF, DCE, CopyProp, CSE
        │   │   │ │   │   Output: Optimized IR
        │   │   │ │   │   ↓
        │   │   │ │   │  [Optimized IR - not used yet in codegen]
        │   │   │ │
        │   │   │ └─→ [CodeGenerator]
        │   │   │     Files: CodeGenerator.h, CodeGenerator.cpp
        │   │   │     Pattern: AST visitor
        │   │   │     Output: C++ source
        │   │   │     ↓
        │   │   │    C++ Output (output.h + output.cpp)
        │   │   │
        │   │   └─→ [Main Driver]
        │   │       File: src/main.cpp
        │   │       Orchestrates all stages
        │   │       Handles CLI and error reporting
        │   │
        │   └──────────────────────────────┘
        │
        └─→ Compilation complete ✓
```

---

## 📋 File Organization Summary

```
src/
├── main.cpp                    # Compiler driver (orchestration)
│
├── regex/                      # Custom Regex Engine
│   ├── RegexParser.h/cpp       # Parse regex patterns → NFA
│   ├── NFA.h/cpp               # Thompson construction
│   └── DFA.h/cpp               # Subset construction, simulation
│
├── lexer/                      # Lexical Analysis (Frontend)
│   ├── Token.h/cpp             # Token definitions
│   └── Lexer.h/cpp             # DFA-based tokenization
│
├── parser/                     # Syntactic Analysis (Frontend)
│   ├── AST.h/cpp               # Abstract Syntax Tree nodes
│   └── Parser.h/cpp            # Recursive descent parser
│
├── ir/                         # Intermediate Representation
│   ├── IR.h/cpp                # IR instruction definitions
│   ├── IRGenerator.h/cpp       # AST → IR conversion (visitor)
│   └── IROptimizer.h/cpp       # IR optimization passes
│
└── codegen/                    # Code Generation (Backend)
    └── CodeGenerator.h/cpp     # IR/AST → C++ conversion (visitor)
```

---

## 🔍 Key Design Patterns

### 1. Visitor Pattern (AST Traversal)

Used in **three places**:

```cpp
// Pattern: Every component that processes AST implements a visitor
class ASTVisitor {
    virtual void visit(ClassDeclaration& node) = 0;
    virtual void visit(MethodDeclaration& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    // ... etc
};

// Implementation 1: Parse validation
class Parser : public ASTVisitor { /* ... */ };

// Implementation 2: IR generation
class IRGenerator : public ASTVisitor { /* ... */ };

// Implementation 3: Code generation
class CodeGenerator : public ASTVisitor { /* ... */ };
```

### 2. Builder Pattern (Token Pattern)

```cpp
struct TokenPattern {
    TokenType type;
    std::unique_ptr<Regex::RegexEngine> regex;  // Built during init
    int priority;
};

// Built in initializePatterns(): 40+ patterns created with priorities
```

### 3. State Machine (DFA)

```cpp
class DFA {
    DFAState* currentState;  // Current state during simulation
    
    bool simulate(const std::string& input) {
        for (char c : input) {
            currentState = currentState->transitions.at(c);
            if (!currentState) return false;
        }
        return currentState->isAccepting;
    }
};
```

---

## 📊 Metrics for Simple.cs

| Stage | Input | Output | Files |
|-------|-------|--------|-------|
| Lexer | 1 source file | 20 tokens | Lexer.h/cpp, Token.h/cpp, regex/* |
| Parser | 20 tokens | 1 AST (13 nodes) | Parser.h/cpp, AST.h/cpp |
| IRGen | 1 AST | ~8 IR instructions | IRGenerator.h/cpp, IR.h/cpp |
| Optimizer | 8 IR instr. | 8 IR instr. (no change) | IROptimizer.h/cpp |
| Codegen | 1 AST | 2 files (44 lines total) | CodeGenerator.h/cpp |

---

## 🎯 Current Status (Month 2)

**What's Working**:
- ✅ Lexical analysis (regex-driven)
- ✅ Syntactic analysis (parser with error recovery)
- ✅ AST construction (all C# OOP features)
- ✅ IR generation (three-address code)
- ✅ IR optimization (4 passes)
- ✅ Code generation (AST → C++ directly)

**What's NOT Yet Connected**:
- ❌ Optimized IR → Code generator (IR optimizations not used in output)

**What's Missing (Month 3+)**:
- ❌ Semantic analyzer (type checking, symbol table)
- ❌ SSA transformation (for advanced optimizations)
- ❌ Register allocation
- ❌ Multiple backends

---

## 🚀 Example Execution

```bash
$ ./bin/cs2cpp examples/simple.cs

Reading input file: examples/simple.cs
Performing lexical analysis...
Found 20 tokens ✓
Performing syntactic analysis...
AST constructed successfully ✓
Generating C++ code...
Writing header to: output.h
Writing source to: output.cpp
Compilation successful! ✓
```

**Output Check**:
```bash
$ cat output.h
#ifndef GENERATED_PROGRAM_H
#define GENERATED_PROGRAM_H
...

$ cat output.cpp
#include "output.h"
...
```

---

## 🔗 Integration Points Between Files

### Lexer → Parser
- **Interface**: Token stream
- **Location**: `src/parser/Parser.cpp::nextToken()`
- **Protocol**: Sequential token consumption

### Parser → IR Generator
- **Interface**: Shared AST
- **Location**: `src/ir/IRGenerator.cpp::visit()` methods
- **Protocol**: Visitor pattern traversal

### IR Generator → IR Optimizer
- **Interface**: IR instruction list
- **Location**: `src/ir/IROptimizer.cpp::optimize()`
- **Protocol**: Multiple passes over instructions

### Parser → Code Generator
- **Interface**: Shared AST (direct path, bypasses IR)
- **Location**: `src/codegen/CodeGenerator.cpp::visit()` methods
- **Protocol**: Visitor pattern traversal

### Code Generator → Output
- **Interface**: Formatted C++ strings
- **Location**: `src/main.cpp::writeOutput()`
- **Protocol**: Write to .h and .cpp files

---

## 📚 How to Trace Through Code

To understand how an input program transforms:

1. **Find the token**: Trace `Lexer.cpp::scanToken()` → returns `Token`
2. **Find AST node**: Trace `Parser.cpp::parse*()` → returns AST node
3. **Find IR instruction**: Trace `IRGenerator.cpp::visit()` → emits IR
4. **Find optimization**: Trace `IROptimizer.cpp::optimize()` → modifies IR
5. **Find C++ output**: Trace `CodeGenerator.cpp::visit()` → writes code

Each file has clear responsibility and single purpose!

---

**Status**: Month 2 Complete  
**Next**: Month 3 will add semantic analysis layer between Parser and Code Generator
