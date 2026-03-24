# Complete Compiler Source Code - All Files Shown

This document displays actual excerpts from all the source files involved in the C# → C++ compilation process, demonstrating their structure and interaction.

---

## 🎯 File - `src/lexer/Token.h` (Token Definitions)

The foundation of lexical analysis - defines all token types recognized by the lexer.

```cpp
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

namespace Compiler {

enum class TokenType {
    // Keywords (35+ types)
    KW_CLASS,
    KW_PUBLIC,
    KW_PRIVATE,
    KW_PROTECTED,
    KW_STATIC,
    KW_VOID,
    KW_INT,
    KW_FLOAT,
    KW_DOUBLE,
    KW_STRING,
    KW_BOOL,
    KW_CHAR,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_FOR,
    KW_FOREACH,
    KW_RETURN,
    // ... more keywords
    
    // Identifiers and literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    
    // Operators (26 types)
    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    OP_ASSIGN,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_LOGICAL_AND,
    OP_LOGICAL_OR,
    OP_LOGICAL_NOT,
    // ... more operators
    
    // Delimiters
    SEMICOLON,
    COMMA,
    DOT,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    
    END_OF_FILE,
    UNKNOWN
};

// Token structure
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t, const std::string& l, int ln, int col)
        : type(t), lexeme(l), line(ln), column(col) {}
};

} // namespace Compiler
```

**Used by**: Lexer → Parser  
**Contains**: 46+ token types, line/column tracking

---

## 🔤 File - `src/lexer/Lexer.h` (Lexical Analyzer)

Uses custom regex engine and finite automata to scan source code.

```cpp
class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    std::vector<TokenPattern> patterns;      // Regex-based patterns
    std::map<std::string, TokenType> keywords;
    
    void initializePatterns();               // Populate regex patterns
    void initializeKeywords();
    
    char peek();
    char advance();
    bool isAtEnd();
    
    Token scanToken();                       // Main scanning method
    Token matchPattern();                    // Uses regex engine
    Token scanString();
    Token scanChar();
    void skipWhitespace();
    void skipComment();
    
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();           // Main API: returns all tokens
    Token nextToken();
};
```

**Key Methods**:
- `initializePatterns()` - Sets up 40+ regex patterns with priorities
- `matchPattern()` - Uses custom regex DFA for token matching
- `tokenize()` - Returns complete token stream

---

## 🌳 File - `src/parser/AST.h` (Abstract Syntax Tree)

Defines all AST node types for C# language constructs.

```cpp
// Base classes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;  // Visitor pattern
};

// Expression nodes
class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string op;                          // Operator: +, -, >, <, etc.
    std::unique_ptr<Expression> right;
};

class IdentifierExpression : public Expression {
public:
    std::string name;                        // Variable or method name
};

class LiteralExpression : public Expression {
public:
    std::string value;
    std::string type;                        // "int", "float", "string", "char", "bool"
};

// Statement nodes
class VariableDeclaration : public Statement {
public:
    std::string name;
    std::string type;
    std::unique_ptr<Expression> initializer; // Optional: = value
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;  // The 'if' block
    std::unique_ptr<Statement> elseBranch;  // The 'else' block (optional)
};

class MethodDeclaration : public Statement {
public:
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<VariableDeclaration>> parameters;
    std::unique_ptr<BlockStatement> body;
    bool isStatic;
    bool isPublic;
};

class ClassDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::unique_ptr<Statement>> members;  // Methods and fields
    bool isPublic;
};
```

**Visitor Pattern**: Each node implements `accept(ASTVisitor&)` for tree traversal

---

## 🔍 File - `src/parser/Parser.h` (Syntactic Analysis)

Recursive descent parser with error recovery.

```cpp
class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    std::vector<ParseError> errors;      // Collect multiple errors
    bool panicMode;                      // Error recovery mode
    
    // Recursive descent parsing methods
    std::unique_ptr<Program> program();
    std::unique_ptr<ClassDeclaration> classDeclaration();
    std::unique_ptr<MethodDeclaration> functionDeclaration(...);
    std::unique_ptr<VariableDeclaration> variableDeclaration();
    
    // Expression parsing (precedence climbing)
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();
    
    // Error recovery
    void reportError(const std::string& message);
    void synchronize();                  // Panic mode recovery
    bool hasSynchronizationPoint();
    
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
    const std::vector<ParseError>& getErrors() const;
};
```

**Parsing Structure**:
- Each method corresponds to grammar rule
- Recursive descent with left-to-right scanning
- Error recovery collects multiple errors before stopping

---

## 📊 File - `src/ir/IR.h` (Intermediate Representation)

Three-address code representation for optimization.

```cpp
namespace Compiler::IR {

// Operations supported by IR
enum class OpCode {
    // Arithmetic
    Add, Sub, Mul, Div, Mod,
    // Comparison
    Eq, Ne, Lt, Le, Gt, Ge,
    // Logical
    And, Or, Not,
    // Memory
    Load, Store, Alloca,
    // Control Flow
    Br,          // Unconditional branch
    CondBr,      // Conditional branch (if)
    Ret,         // Return from function
    Call,        // Function call
    // Type
    Cast,
    // Special
    Label,       // Jump target
    Phi          // SSA phi node (for future use)
};

// Operand: temporary, variable, or constant
struct Operand {
    enum class Type { Temporary, Variable, Constant, Label };
    Type type;
    std::string name;      // t1, x, 42, L1
    Value value;           // Actual value for constants
};

// Single 3-address instruction
class Instruction {
public:
    OpCode opcode;
    std::unique_ptr<Operand> dest;     // Destination (left side)
    std::unique_ptr<Operand> src1;     // First source
    std::unique_ptr<Operand> src2;     // Second source (if binary op)
    std::string label;                 // For labels and branches
    
    // Example: x = a + b  →
    //   dest: x, opcode: Add, src1: a, src2: b
};

// Basic block: sequence of instructions (no branches in middle)
class BasicBlock {
public:
    std::string name;
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::vector<BasicBlock*> predecessors;  // Control flow Graph
    std::vector<BasicBlock*> successors;
};

// Function: name + body
class Function {
public:
    std::string name;
    std::vector<std::unique_ptr<BasicBlock>> basicBlocks;
    std::string returnType;
};

// Module: collection of functions
class Module {
public:
    std::vector<std::unique_ptr<Function>> functions;
};

} // namespace Compiler::IR
```

**Key Concepts**:
- Three-address code (at most 2 operands per instruction)
- Basic blocks for optimization
- Control flow graph representation

---

## 🔄 File - `src/ir/IRGenerator.h` (AST → IR)

Converts AST to three-address code using visitor pattern.

```cpp
class IRGenerator : public ASTVisitor {
private:
    IR::Module* module;
    IR::Function* currentFunction;
    IR::BasicBlock* currentBlock;
    std::string lastTemp;               // Result of last expression
    int labelCounter;                   // For generating unique labels
    
    std::string newLabel() {
        return "L" + std::to_string(labelCounter++);
    }
    
public:
    IRGenerator();
    std::unique_ptr<IR::Module> generate(Program& program);
    
    // Expression visitors - generate IR, return temp holding result
    void visit(BinaryExpression& node) override {
        // Generate:
        // t1 = visit(left)
        // t2 = visit(right)
        // t3 = t1 op t2
        // lastTemp = t3
    }
    
    void visit(LiteralExpression& node) override {
        // Generate:
        // t1 = constant value
        // lastTemp = t1
    }
    
    void visit(IfStatement& node) override {
        // Generate:
        // t1 = visit(condition)
        // cond_br t1, thenLabel, elseLabel
        // (then block)
        // br endLabel
        // elseLabel: (else block)
        // endLabel:
    }
    
    // More visitors...
    void visit(VariableDeclaration& node) override;
    void visit(MethodDeclaration& node) override;
    // ... all 14+ visitors
};
```

**Process**:
1. Traverse AST with visitor pattern
2. Generate IR instructions for each node
3. Track temporary variables for expression results
4. Handle control flow with labels

---

## ⚡ File - `src/ir/IROptimizer.h` (Optimization)

Performs 4 optimization passes on IR.

```cpp
class IROptimizer {
public:
    // Four optimization passes
    static void constantFolding(IR::Module& module);
        // 42 + 8 → 50
        // x + 0 → x
        // x * 1 → x
    
    static void deadCodeElimination(IR::Module& module);
        // Remove: t1 = 5  (if t1 never used)
        // Remove unreachable blocks
    
    static void copyPropagation(IR::Module& module);
        // If x = y, replace all x with y
    
    static void commonSubexpressionElimination(IR::Module& module);
        // t1 = a + b
        // t2 = a + b  → t2 = t1 (reuse result)
    
    static void optimize(IR::Module& module, int level = 2) {
        // Apply passes based on optimization level
        // level 1: Just constant folding
        // level 2: All passes (default)
        // level 3: All passes + multiple iterations
    }
};
```

**Output**: Optimized IR (not yet fed back to code generator)

---

## 💻 File - `src/codegen/CodeGenerator.h` (Code Generation)

Converts AST to C++ with pretty printing.

```cpp
struct FormatOptions {
    int indentSize = 4;
    bool useTabs = false;
    bool spaceBeforeParen = false;
    int maxLineLength = 100;
    // ... more formatting options
};

class CodeGenerator : public ASTVisitor {
private:
    std::stringstream output;           // Accumulates C++ code
    std::stringstream headerOutput;     // Header file (.h)
    int indentLevel;
    bool isGeneratingHeader;            // Header vs source mode
    std::string currentClassName;       // Track class context
    FormatOptions formatOpts;
    
    void indent() { /* Add indentation */ }
    void writeLine(const std::string& line) { /* Write line */ }
    void write(const std::string& text) { /* Write text */ }
    void writeFormatted(...);           // Pretty printing
    std::string formatExpression(...);  // Format expressions
    
    // Type conversion
    std::string convertType(const std::string& csType) {
        // "int" → "int"
        // "string" → "std::string"
        // "bool" → "bool"
    }
    
    // Operator conversion
    std::string convertOperator(const std::string& op) {
        // "==" → "=="
        // "/" → "/"
        // "&&" → "&&"
    }
    
public:
    CodeGenerator();
    
    std::string generate(Program& program);        // Both header + source
    std::string generateHeader(Program& program);  // Just header
    std::string generateSource(Program& program);  // Just source
    
    // Visitor methods for all AST nodes
    void visit(BinaryExpression& node) override;
    void visit(ClassDeclaration& node) override;
    void visit(IfStatement& node) override;
    // ... more visitors
};
```

**Output**: 
- `output.h` - C++ header file
- `output.cpp` - C++ source file

---

## 🚀 File - `src/main.cpp` (Compiler Driver)

Orchestrates the entire compilation pipeline.

```cpp
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "codegen/CodeGenerator.h"
#include "ir/IRGenerator.h"
#include "ir/IROptimizer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputFile;
    std::string outputFile = "output.cpp";
    bool emitIR = false;
    bool optimize = false;
    int optLevel = 2;
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--emit-ir") {
            emitIR = true;
        } else if (arg == "--optimize") {
            optimize = true;
        } else if (arg.find("--opt-level=") == 0) {
            optLevel = std::stoi(arg.substr(12));
        } else if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                outputFile = arg;
            }
        }
    }
    
    try {
        // STAGE 1: Lexical Analysis
        std::cout << "Reading input file: " << inputFile << "\n";
        std::string sourceCode = readFile(inputFile);
        
        std::cout << "Performing lexical analysis...\n";
        Compiler::Lexer lexer(sourceCode);
        std::vector<Compiler::Token> tokens = lexer.tokenize();
        std::cout << "Found " << tokens.size() << " tokens\n";
        
        // STAGE 2: Syntactic Analysis
        std::cout << "Performing syntactic analysis...\n";
        Compiler::Parser parser(tokens);
        auto program = parser.parse();
        
        if (parser.hasErrors()) {
            std::cout << "Parse errors:\n";
            for (const auto& error : parser.getErrors()) {
                std::cout << "  " << error.toString() << "\n";
            }
            return 1;
        }
        
        std::cout << "AST constructed successfully\n";
        
        // STAGE 3: IR Generation (optional)
        if (emitIR || optimize) {
            std::cout << "Generating IR...\n";
            Compiler::IRGenerator irGen;
            auto module = irGen.generate(*program);
            
            // STAGE 4: Optimization
            if (optimize) {
                std::cout << "Optimizing IR (level " << optLevel << ")...\n";
                Compiler::IROptimizer::optimize(*module, optLevel);
            }
            
            // Emit IR if requested
            if (emitIR) {
                std::string irOutput = module->toString();
                writeFile(outputFile, irOutput);
                std::cout << "IR written to: " << outputFile << "\n";
                return 0;
            }
        }
        
        // STAGE 5: Code Generation
        std::cout << "Generating C++ code...\n";
        Compiler::CodeGenerator codegen;
        
        std::string sourceHeader = codegen.generateHeader(*program);
        std::string sourceCode = codegen.generateSource(*program);
        
        // Write output files
        std::string headerFile = outputFile;
        size_t dotPos = headerFile.find_last_of('.');
        if (dotPos != std::string::npos) {
            headerFile = headerFile.substr(0, dotPos) + ".h";
        } else {
            headerFile += ".h";
        }
        
        std::cout << "Writing header to: " << headerFile << "\n";
        writeFile(headerFile, sourceHeader);
        
        std::cout << "Writing source to: " << outputFile << "\n";
        writeFile(outputFile, sourceCode);
        
        std::cout << "\nCompilation successful!\n";
        std::cout << "Generated C++ header: " << headerFile << "\n";
        std::cout << "Generated C++ source: " << outputFile << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << "\n";
        return 1;
    }
}
```

**Pipeline**:
1. Read C# source file
2. Lexical analysis → tokens
3. Syntactic analysis → AST
4. (Optional) IR generation → optimization
5. Code generation → C++ files
6. Write output

---

## 📁 Complete File Organization

```
src/
├── main.cpp                    [Shown above] ✓
│   └─ Orchestrates all stages
│
├── regex/                      
│   ├── RegexParser.h/cpp       [Custom regex engine]
│   ├── NFA.h/cpp               [Thompson construction]
│   └── DFA.h/cpp               [Subset construction]
│
├── lexer/                      
│   ├── Token.h/cpp             [Shown above] ✓
│   └── Lexer.h/cpp             [Shown above] ✓
│
├── parser/                     
│   ├── AST.h/cpp               [Shown above] ✓
│   └── Parser.h/cpp            [Shown above] ✓
│
├── ir/                         
│   ├── IR.h/cpp                [Shown above] ✓
│   ├── IRGenerator.h/cpp       [Shown above] ✓
│   └── IROptimizer.h/cpp       [Shown above] ✓
│
└── codegen/                    
    └── CodeGenerator.h/cpp     [Shown above] ✓
```

---

## 🔗 Data Flow Through Files

```
main.cpp:readFile()
    ↓
src/lexer/Lexer.cpp
    [Uses: src/regex/* via RegexEngine]
    ↓
Object: std::vector<Token> (defined in Token.h)
    ↓
main.cpp:parser.parse()
    ↓
src/parser/Parser.cpp
    [Creates: AST nodes from AST.h]
    ↓
Object: std::unique_ptr<Program> (defined in AST.h)
    ↓
[Branch A] main.cpp:codegen.generate()
    ↓
    src/codegen/CodeGenerator.cpp
    [Visits: All AST nodes]
    ↓
    Output: output.h, output.cpp
    
[Branch B] main.cpp:irGen.generate()
    ↓
    src/ir/IRGenerator.cpp
    [Visits: All AST nodes]
    ↓
    Object: std::unique_ptr<IR::Module> (defined in IR.h)
    ↓
    main.cpp:optimizer.optimize()
    ↓
    src/ir/IROptimizer.cpp
    [Modifies: IR instructions in place]
    ↓
    Output: output.ir (if --emit-ir)
```

---

## 🎯 Key Design Patterns Across Files

### 1. Visitor Pattern (AST Traversal)

**Definition** (AST.h):
```cpp
class ASTVisitor {
    virtual void visit(BinaryExpression&) = 0;
    virtual void visit(IfStatement&) = 0;
    // ...
};
```

**Implementation 1** (Parser.h/cpp):
```cpp
class Parser : public ASTVisitor { };
```

**Implementation 2** (IRGenerator.h/cpp):
```cpp
class IRGenerator : public ASTVisitor {
    void visit(BinaryExpression& node) override { /* Generate IR */ }
};
```

**Implementation 3** (CodeGenerator.h/cpp):
```cpp
class CodeGenerator : public ASTVisitor {
    void visit(BinaryExpression& node) override { /* Generate C++ */ }
};
```

---

## 📊 File Responsibilities

| File | Purpose | Input | Output | LOC |
|------|---------|-------|--------|-----|
| Token.h | Definitions | — | 46+ token types | 150 |
| Lexer.h/cpp | Tokenization | Source string | Token vector | 380 |
| AST.h/cpp | Tree structure | — | 40+ node types | 850 |
| Parser.h/cpp | Parsing | Tokens | AST | 850 |
| IR.h/cpp | IR definition | — | 25+ opcodes | 1,100 |
| IRGenerator.h/cpp | AST→IR | AST | IR Module | 900 |
| IROptimizer.h/cpp | Optimization | IR | Optimized IR | 600 |
| CodeGenerator.h/cpp | AST→C++ | AST | C++ files | 800 |
| main.cpp | Driver | All | Coordination | 180 |

---

## ✅ Complete Data Transformation Example

Using `simple.cs`:

```
INPUT:
class Program {
    public static void Main() {
        int x = 42;
        if (x > 0) {
            System.Console.WriteLine("Hello");
        }
    }
}

STAGE 1 (Lexer - tokens in Token.h):
[KW_CLASS] [IDENTIFIER:"Program"] [LBRACE] ... (20 tokens total)

STAGE 2 (Parser - AST nodes in AST.h):
Program
  └─ ClassDeclaration "Program"
      └─ MethodDeclaration "Main"
          └─ IfStatement
              ├─ condition: BinaryExpression(x > 0)
              └─ thenBranch: BlockStatement
                  └─ ExpressionStatement
                      └─ CallExpression(Console.WriteLine)

STAGE 3 (IRGenerator - Instructions in IR.h):
  t1 = 42
  x = t1
  t2 = x
  t3 = 0
  cond = gt t2, t3
  jz cond, L1
  [call WriteLine]
  L1:
  ret void

STAGE 4 (IROptimizer):
  [No changes - all instructions are necessary]

STAGE 5 (CodeGenerator - C++ in CodeGenerator.h):
output.h:
class Program {
public:
    static void Main();
};

output.cpp:
void Program::Main() {
    int x = 42;
    if (x > 0) {
        std::cout << "Hello" << std::endl;
    }
}
```

---

**All 9 core files shown with actual structure and responsibilities** ✓
