# Month 3 Quick Start Guide
## Semantic Analysis & Type System Implementation

**Duration**: Weeks 9-12 (4 weeks)  
**Goal**: Add robust type checking, symbol tables, and semantic validation  
**Estimated Work**: ~1,200 LOC

---

## 🎯 Executive Summary

This month transforms the compiler from a syntax translator to a semantically-aware system. You'll implement:
1. **Symbol Table** - Track all declarations with scope hierarchy
2. **Type System** - Represent and check types at compile time
3. **Semantic Analyzer** - Validate correctness beyond syntax

By the end of Month 3, the compiler will catch type errors, validate method calls, and ensure semantic correctness.

---

## 📋 Week-by-Week Breakdown

### Week 9: Symbol Table Foundation (Days 1-7)

**Day 1-2: Design & Planning**
- [ ] Read about symbol table design (Dragon Book Ch. 2.7)
- [ ] Sketch scope hierarchy structure
- [ ] Design symbol entry format
- [ ] Plan name resolution algorithm

**Day 3-5: Implementation**
- [ ] Create `src/semantic/SymbolTable.h`
- [ ] Implement scope stack (vector of maps)
- [ ] Add enter_scope() / exit_scope() methods
- [ ] Implement insert() / lookup() / lookup_all_scopes()

**Day 6-7: Testing**
- [ ] Create `tests/test_symbol_table.cpp`
- [ ] Test nested scopes
- [ ] Test name shadowing
- [ ] Test qualified name resolution

**Deliverable**: Working symbol table with scope management

---

### Week 10: Type System Implementation (Days 8-14)

**Day 8-9: Type Representation**
- [ ] Create `src/semantic/Type.h`
- [ ] Implement Type base class
- [ ] Create PrimitiveType, ClassType, InterfaceType, GenericType
- [ ] Add type comparison methods (equals, is_assignable_to)

**Day 10-12: Type Operations**
- [ ] Implement type equivalence checking
- [ ] Add implicit conversion rules (int → float, derived → base)
- [ ] Handle generic type instantiation
- [ ] Create type registry (built-in types)

**Day 13-14: Testing**
- [ ] Test type equivalence
- [ ] Test type compatibility
- [ ] Test generic type handling
- [ ] Test inheritance relationships

**Deliverable**: Complete type system with comparison and conversion

---

### Week 11: Semantic Analyzer - Part 1 (Days 15-21)

**Day 15-16: Infrastructure**
- [ ] Create `src/semantic/SemanticAnalyzer.h`
- [ ] Inherit from ASTVisitor
- [ ] Add symbol table and current scope tracking
- [ ] Implement visitor pattern for expression type checking

**Day 17-19: Expression Type Checking**
- [ ] visit(BinaryExpression&) - check operand types
- [ ] visit(UnaryExpression&) - validate unary operations
- [ ] visit(Identifier&) - lookup symbol, return type
- [ ] visit(Literal&) - return literal type
- [ ] visit(CallExpression&) - validate arguments

**Day 20-21: Statement Checking**
- [ ] visit(VariableDeclaration&) - insert into symbol table
- [ ] visit(AssignmentExpression&) - check assignment compatibility
- [ ] visit(ReturnStatement&) - validate return type
- [ ] visit(IfStatement&) - require boolean condition

**Deliverable**: Semantic analyzer checking expressions and basic statements

---

### Week 12: Semantic Analyzer - Part 2 & Integration (Days 22-28)

**Day 22-24: Advanced Checking**
- [ ] visit(FunctionDeclaration&) - check signature, return paths
- [ ] visit(ClassDeclaration&) - validate members, inheritance
- [ ] visit(InterfaceDeclaration&) - check interface implementation
- [ ] Method overload resolution

**Day 25-26: Integration**
- [ ] Update main.cpp to run semantic analysis after parsing
- [ ] Add --check-only flag (parse + semantic check, no codegen)
- [ ] Collect and report semantic errors
- [ ] Update error messages with type information

**Day 27-28: Comprehensive Testing**
- [ ] Create `tests/test_semantic.cpp` with 20+ tests
- [ ] Test type errors (assignment, function calls, returns)
- [ ] Test scope errors (undefined variables, duplicate declarations)
- [ ] Test inheritance and interface implementation
- [ ] Update integration tests with semantic checks

**Deliverable**: Complete semantic analysis integrated into compiler pipeline

---

## 🏗️ Code Structure

### New Directory Structure
```
src/
├── semantic/              # NEW
│   ├── SymbolTable.h
│   ├── SymbolTable.cpp
│   ├── Type.h
│   ├── Type.cpp
│   ├── SemanticAnalyzer.h
│   └── SemanticAnalyzer.cpp
tests/
├── test_symbol_table.cpp  # NEW
├── test_types.cpp         # NEW
└── test_semantic.cpp      # NEW
examples/
├── type_errors.cs         # NEW - should fail semantic check
└── scope_test.cs          # NEW - scope and shadowing
```

---

## 💻 Implementation Templates

### Symbol Table (SymbolTable.h)
```cpp
#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "Type.h"

namespace Compiler {
namespace Semantic {

struct Symbol {
    std::string name;
    std::shared_ptr<Type> type;
    enum Kind { Variable, Function, Class, Parameter } kind;
    int line;  // Declaration line for error messages
    
    Symbol(const std::string& name, std::shared_ptr<Type> type, Kind kind, int line = 0)
        : name(name), type(type), kind(kind), line(line) {}
};

class Scope {
    std::map<std::string, Symbol> symbols;
    std::string name;  // For debugging
    
public:
    Scope(const std::string& name = "global") : name(name) {}
    
    bool insert(const std::string& name, const Symbol& symbol);
    Symbol* lookup(const std::string& name);
    bool contains(const std::string& name) const;
};

class SymbolTable {
    std::vector<Scope> scopes;  // Stack of scopes
    
public:
    SymbolTable();
    
    void enter_scope(const std::string& name = "");
    void exit_scope();
    
    bool insert(const std::string& name, const Symbol& symbol);
    Symbol* lookup(const std::string& name);  // Search current scope
    Symbol* lookup_all_scopes(const std::string& name);  // Search scope chain
    
    int current_depth() const { return scopes.size(); }
};

}} // namespace Compiler::Semantic
```

### Type System (Type.h)
```cpp
#pragma once
#include <string>
#include <memory>
#include <vector>

namespace Compiler {
namespace Semantic {

class Type {
public:
    enum TypeKind {
        Primitive,  // int, float, bool, void
        Class,      // User-defined classes
        Interface,  // Interfaces
        Generic,    // Template/Generic types
        Array,      // Array types
        Function    // Function types (for delegates)
    };
    
    virtual ~Type() = default;
    virtual TypeKind get_kind() const = 0;
    virtual std::string to_string() const = 0;
    virtual bool equals(const Type& other) const = 0;
    virtual bool is_assignable_to(const Type& target) const = 0;
};

class PrimitiveType : public Type {
    enum PrimitiveKind { Int, Float, Double, Bool, Char, String, Void };
    PrimitiveKind kind;
    
public:
    PrimitiveType(PrimitiveKind kind) : kind(kind) {}
    
    TypeKind get_kind() const override { return TypeKind::Primitive; }
    std::string to_string() const override;
    bool equals(const Type& other) const override;
    bool is_assignable_to(const Type& target) const override;
    
    static std::shared_ptr<PrimitiveType> Int() {
        return std::make_shared<PrimitiveType>(PrimitiveKind::Int);
    }
    static std::shared_ptr<PrimitiveType> Bool() {
        return std::make_shared<PrimitiveType>(PrimitiveKind::Bool);
    }
    // ... similar for other primitives
};

class ClassType : public Type {
    std::string name;
    std::shared_ptr<ClassType> base_class;
    std::vector<std::string> interfaces;
    
public:
    ClassType(const std::string& name) : name(name) {}
    
    TypeKind get_kind() const override { return TypeKind::Class; }
    std::string to_string() const override { return name; }
    bool equals(const Type& other) const override;
    bool is_assignable_to(const Type& target) const override;
    
    void set_base_class(std::shared_ptr<ClassType> base) { base_class = base; }
    void add_interface(const std::string& iface) { interfaces.push_back(iface); }
};

class TypeRegistry {
    static std::map<std::string, std::shared_ptr<Type>> types;
    
public:
    static void initialize();  // Register built-in types
    static std::shared_ptr<Type> get(const std::string& name);
    static void register_type(const std::string& name, std::shared_ptr<Type> type);
};

}} // namespace Compiler::Semantic
```

### Semantic Analyzer (SemanticAnalyzer.h)
```cpp
#pragma once
#include "../parser/AST.h"
#include "SymbolTable.h"
#include "Type.h"
#include <vector>
#include <string>

namespace Compiler {
namespace Semantic {

struct SemanticError {
    std::string message;
    int line;
    int column;
    
    SemanticError(const std::string& msg, int line, int col)
        : message(msg), line(line), column(col) {}
        
    std::string to_string() const;
};

class SemanticAnalyzer : public ASTVisitor {
    SymbolTable symbol_table;
    std::vector<SemanticError> errors;
    std::shared_ptr<Type> current_function_return_type;
    std::shared_ptr<Type> last_expression_type;
    
    void report_error(const std::string& message, int line = 0, int col = 0);
    
public:
    SemanticAnalyzer();
    
    void analyze(Program& program);
    bool has_errors() const { return !errors.empty(); }
    const std::vector<SemanticError>& get_errors() const { return errors; }
    
    // Visitor methods
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(Identifier& node) override;
    void visit(IntegerLiteral& node) override;
    void visit(FloatLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(BooleanLiteral& node) override;
    void visit(CallExpression& node) override;
    void visit(MemberExpression& node) override;
    
    void visit(VariableDeclaration& node) override;
    void visit(AssignmentExpression& node) override;
    void visit(ExpressionStatement& node) override;
    void visit(BlockStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ReturnStatement& node) override;
    
    void visit(FunctionDeclaration& node) override;
    void visit(ClassDeclaration& node) override;
    void visit(InterfaceDeclaration& node) override;
    void visit(Program& node) override;
};

}} // namespace Compiler::Semantic
```

---

## 🧪 Testing Strategy

### Test-Driven Development Approach

**For each component, write tests FIRST:**

1. **Symbol Table Tests** (20 tests)
   - Scope creation/destruction
   - Symbol insertion
   - Name lookup in current scope
   - Name lookup in parent scopes
   - Name shadowing
   - Duplicate declaration detection

2. **Type System Tests** (20 tests)
   - Type equality
   - Primitive type compatibility
   - Class inheritance relationships
   - Interface implementation checking
   - Generic type instantiation
   - Array types

3. **Semantic Analysis Tests** (25 tests)
   - Type mismatch in assignment
   - Type mismatch in binary operations
   - Undefined variable usage
   - Function call with wrong arguments
   - Return type mismatch
   - Unreachable code detection
   - Interface implementation validation

---

## 📚 Learning Resources

### Required Reading (Week 9)
1. **Dragon Book**: Chapter 2.7 (Symbol Tables), Chapter 6 (Type Checking)
2. **LLVM Tutorial**: Type System design (online)
3. **Appel's Modern Compiler**: Chapter 5 (Semantic Analysis)

### Recommended Papers
- "Type Inference for ML" - Robin Milner
- "Principal Type-Schemes for Functional Programs" - Damas & Milner

### Reference Implementations
- Study Roslyn (C# compiler) symbol table implementation
- Review Clang's AST type system
- Look at Java compiler (javac) semantic analysis

---

## ⚡ Quick Start Commands

### Day 1: Set Up Structure
```bash
# Create directories
mkdir -p src/semantic
mkdir -p tests/semantic
mkdir -p examples/semantic_tests

# Create initial files
touch src/semantic/SymbolTable.h
touch src/semantic/SymbolTable.cpp
touch src/semantic/Type.h
touch src/semantic/Type.cpp
touch tests/test_symbol_table.cpp

# Update Makefile
# Add SEMANTIC_SOURCES = src/semantic/*.cpp
# Update SOURCES to include SEMANTIC_SOURCES
```

### Week 9 Goal Test
```bash
# Compile and run symbol table tests
g++ -std=c++17 -I./src -o tests/test_symbol_table \
    tests/test_symbol_table.cpp \
    src/semantic/SymbolTable.cpp

./tests/test_symbol_table
# Expected: ✓ All symbol table tests passed (20/20)
```

---

## 🎯 Success Criteria for Month 3

### Week 9 Completion
- [ ] Symbol table implements all required operations
- [ ] Scope hierarchy works correctly
- [ ] 20/20 symbol table tests pass
- [ ] ~300 LOC added

### Week 10 Completion
- [ ] Type system represents all C# types
- [ ] Type comparison and compatibility work
- [ ] 20/20 type system tests pass
- [ ] ~400 LOC added (cumulative: ~700)

### Week 11 Completion
- [ ] Semantic analyzer checks expressions
- [ ] Basic statement validation works
- [ ] Type errors are detected and reported
- [ ] ~300 LOC added (cumulative: ~1,000)

### Week 12 Completion
- [ ] All AST nodes have semantic checks
- [ ] Semantic analysis integrated in compiler pipeline
- [ ] 25+ semantic tests pass
- [ ] All 10 example programs pass semantic analysis
- [ ] ~200 LOC added (cumulative: ~1,200)
- [ ] Documentation updated (SEMANTIC_ANALYSIS.md)

**Final Deliverable**: Compiler with complete semantic analysis  
**Total Additional LOC**: ~1,200  
**New Test Count**: +65 tests  
**Project Total**: ~5,850 LOC, 89 tests

---

## 🐛 Common Pitfalls to Avoid

1. **Don't mix lexical scope with semantic scope**
   - Lexical: Where variable is written in code
   - Semantic: Where variable is accessible at runtime

2. **Handle generic types correctly**
   - Generic<int> and Generic<float> are different types
   - Need type instantiation tracking

3. **Remember method overload resolution**
   - Multiple methods with same name but different signatures
   - Must select best match based on argument types

4. **Don't forget about implicit conversions**
   - int → float is allowed
   - Derived → Base is allowed
   - But not vice versa

5. **Track current function return type**
   - Needed to validate return statements
   - Must handle nested functions (if added)

---

## 📞 Getting Help

If you get stuck:

1. **Review the templates above** - They provide starting structure
2. **Check Dragon Book** - Authoritative reference for symbol tables
3. **Study existing implementations** - Roslyn, Clang have good examples
4. **Test incrementally** - Don't wait until everything is done
5. **Ask for code review** - Early feedback prevents major refactoring

---

**Ready to start? Begin with Week 9, Day 1:**
```bash
# Your first task
mkdir -p src/semantic
code src/semantic/SymbolTable.h
```

**Next milestone**: Symbol table with 20 passing tests (7 days)

Good luck! 🚀
