# Compiler Architecture Evolution
## Visual Guide to 6-Month Development

This document shows the architectural evolution of the compiler from Month 2 (current) to Month 6 (completion).

---

## 🏗️ Month 2 Architecture (CURRENT - COMPLETED)

```
┌─────────────────────────────────────────────────────────────┐
│                       C# Source Code                         │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Custom Regex Engine        │
        │  (Thompson Construction)     │
        │  (Subset Construction)       │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Lexical Analyzer (DFA)     │
        │   • 46+ token types          │
        │   • Manual construction      │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Recursive Descent Parser   │
        │   • Error recovery           │
        │   • Multiple error reporting │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Abstract Syntax Tree       │
        │   • Full C# OOP support      │
        │   • Inheritance, Interfaces  │
        └──────────────┬───────────────┘
                       │
                       ├──────────────────────────────┐
                       │                              │
                       ▼                              ▼
        ┌──────────────────────────┐   ┌──────────────────────────┐
        │   IR Generator           │   │   Code Generator         │
        │   • Three-address code   │   │   • Direct AST→C++       │
        │   • Basic blocks         │   │   • Header/Source split  │
        └──────────┬───────────────┘   │   • Pretty printing      │
                   │                   └────────┬─────────────────┘
                   ▼                            │
        ┌──────────────────────────┐           │
        │   IR Optimizer           │           │
        │   • Constant folding     │           │
        │   • Dead code elim.      │           │
        │   • Copy propagation     │           │
        │   • CSE                  │           │
        └──────────┬───────────────┘           │
                   │                            │
                   ▼                            ▼
        [Optimized IR]              ┌──────────────────────┐
        (not connected               │   C++ Output Files   │
         to codegen yet)             │   • .h header        │
                                     │   • .cpp source      │
                                     └──────────────────────┘
```

**Key Characteristics:**
- ✅ Two parallel paths: IR pipeline (for optimization) and direct C++ generation
- ✅ IR optimization works but doesn't affect C++ output yet
- ✅ ~4,650 lines of code
- ✅ 24 tests passing

---

## 🎯 Month 3 Architecture (PLANNED - Semantic Analysis)

```
┌─────────────────────────────────────────────────────────────┐
│                       C# Source Code                         │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Lexer (unchanged)          │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Parser (unchanged)         │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Abstract Syntax Tree       │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌══════════════════════════════┐  ★ NEW ★
        ║   SEMANTIC ANALYZER          ║
        ║                              ║
        ║   ┌────────────────────┐     ║
        ║   │  Symbol Table      │     ║
        ║   │  • Scope hierarchy │     ║
        ║   │  • Name resolution │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   ┌────────────────────┐     ║
        ║   │  Type System       │     ║
        ║   │  • Type checking   │     ║
        ║   │  • Compatibility   │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   ┌────────────────────┐     ║
        ║   │  Validation        │     ║
        ║   │  • Method calls    │     ║
        ║   │  • Assignments     │     ║
        ║   │  • Returns         │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   [Annotated AST with types] ║
        ╚══════════════╤═══════════════╝
                       │
                       ├──────────────────────────────┐
                       │                              │
                       ▼                              ▼
        (IR Generator - unchanged)    (Code Generator - unchanged)
```

**Additions:**
- ✅ Symbol table with scope management (+400 LOC)
- ✅ Type system with checking (+400 LOC)
- ✅ Semantic analyzer (+400 LOC)
- ✅ 65+ new tests
- **Total: ~5,850 LOC**

---

## 🚀 Month 4 Architecture (PLANNED - Advanced Features)

```
[Same as Month 3, with AST enhancements]

        ┌══════════════════════════════┐
        ║   ENHANCED AST               ║
        ║                              ║
        ║   ┌────────────────────┐     ║  ★ NEW ★
        ║   │  Constructors      │     ║
        ║   │  • Default ctor    │     ║
        ║   │  • Param ctor      │     ║
        ║   │  • Base calls      │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   ┌────────────────────┐     ║  ★ NEW ★
        ║   │  Properties        │     ║
        ║   │  • get/set         │     ║
        ║   │  • Auto-props      │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   ┌────────────────────┐     ║  ★ NEW ★
        ║   │  Operators         │     ║
        ║   │  • Binary ops      │     ║
        ║   │  • Unary ops       │     ║
        ║   │  • Conversions     │     ║
        ║   └────────────────────┘     ║
        ╚══════════════════════════════╝
```

**Additions:**
- ✅ Constructor/destructor AST nodes (+300 LOC)
- ✅ Property AST nodes (+200 LOC)
- ✅ Operator overloading (+500 LOC)
- ✅ Enhanced code generation (+200 LOC)
- **Total: ~6,850 LOC**

---

## 🔬 Month 5 Architecture (PLANNED - Advanced Optimization)

```
[Lexer → Parser → Semantic Analysis → AST]
                       │
                       ▼
        ┌══════════════════════════════┐
        ║   ENHANCED IR GENERATOR      ║
        ║   • SSA transformation       ║  ★ NEW ★
        ║   • Phi node insertion       ║
        ║   • Variable renaming        ║
        ╚══════════════╤═══════════════╝
                       │
                       ▼
        ┌══════════════════════════════┐
        ║   SSA-FORM IR                ║
        ║   • Static single assignment ║  ★ NEW ★
        ║   • Phi nodes at merges      ║
        ║   • Def-use chains           ║
        ╚══════════════╤═══════════════╝
                       │
                       ▼
        ┌══════════════════════════════┐
        ║   ADVANCED IR OPTIMIZER      ║  ★ ENHANCED ★
        ║                              ║
        ║   ┌────────────────────┐     ║
        ║   │  Dataflow Analysis │     ║  ★ NEW ★
        ║   │  • Reaching defs   │     ║
        ║   │  • Live vars       │     ║
        ║   │  • Avail exprs     │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   ┌────────────────────┐     ║
        ║   │  Loop Analysis     │     ║  ★ NEW ★
        ║   │  • Loop detection  │     ║
        ║   │  • LICM            │     ║
        ║   │  • Ind var optim   │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   ┌────────────────────┐     ║
        ║   │  SSA Optimizations │     ║  ★ NEW ★
        ║   │  • SCCP            │     ║
        ║   │  • GVN             │     ║
        ║   │  • Aggressive DCE  │     ║
        ║   └────────────────────┘     ║
        ║                              ║
        ║   [Highly optimized SSA IR]  ║
        ╚══════════════╤═══════════════╝
                       │
                       ▼
        [Still not connected to C++ gen]
```

**Additions:**
- ✅ SSA transformation (+600 LOC)
- ✅ Dataflow analysis framework (+400 LOC)
- ✅ Loop analysis & optimization (+600 LOC)
- ✅ SSA-based optimizations (+200 LOC)
- **Total: ~9,850 LOC**

---

## 🎯 Month 6 Architecture (FINAL - Complete Pipeline)

```
┌─────────────────────────────────────────────────────────────┐
│                       C# Source Code                         │
└──────────────────────┬──────────────────────────────────────┘
                       │
                [Lexer → Parser → Semantic Analysis → AST]
                       │
                       ▼
        ┌──────────────────────────────┐
        │   IR Generator (SSA)         │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌──────────────────────────────┐
        │   Advanced IR Optimizer      │
        │   (All optimization passes)  │
        └──────────────┬───────────────┘
                       │
                       ▼
        ┌══════════════════════════════┐
        ║   REGISTER ALLOCATION        ║  ★ NEW ★
        ║   • Graph coloring           ║
        ║   • Linear scan              ║
        ║   • Liveness analysis        ║
        ║   • Spill code generation    ║
        ╚══════════════╤═══════════════╝
                       │
                       ▼
        ┌══════════════════════════════┐
        ║   BACKEND SELECTION          ║  ★ NEW ★
        ╚══════════════╤═══════════════╝
                       │
         ┌─────────────┼─────────────┐
         │             │             │
         ▼             ▼             ▼
   ┌─────────┐  ┌──────────┐  ┌──────────┐
   │ C++     │  │ LLVM IR  │  │ Future:  │
   │ Backend │  │ Backend  │  │ Native   │
   │         │  │(Optional)│  │ ...      │
   └────┬────┘  └────┬─────┘  └──────────┘
        │            │
        ▼            ▼
   ┌─────────┐  ┌─────────┐
   │ .h/.cpp │  │ .ll     │
   │ files   │  │ file    │
   └─────────┘  └────┬────┘
                     │
                     ▼
                ┌─────────┐
                │  llvm   │
                │ compiler│
                └────┬────┘
                     │
                     ▼
                ┌─────────┐
                │ Native  │
                │ binary  │
                └─────────┘
```

**Complete Pipeline:**
```
C# Source
    ↓
Lexer (DFA-based)
    ↓
Parser (Recursive descent with error recovery)
    ↓
Semantic Analyzer (Type checking, symbol table)
    ↓
IR Generator (Three-address code → SSA form)
    ↓
IR Optimizer (Dataflow, loops, SSA optimizations)
    ↓
Register Allocator (Graph coloring)
    ↓
Backend (C++ or LLVM)
    ↓
Target Code (.cpp or .ll or native)
```

**Additions:**
- ✅ IR→C++ code generator (+700 LOC)
- ✅ Register allocation (+400 LOC)
- ✅ Delegates & lambdas (+300 LOC)
- ✅ LLVM backend (optional) (+800 LOC)
- ✅ Backend selection (+100 LOC)
- **Total: ~11,450 LOC**

---

## 📊 Architectural Metrics Progression

### Code Organization by Month

| Month | Lexer | Parser | Semantic | IR | Codegen | Total |
|-------|-------|--------|----------|----|---------| ------|
| 2     | 1,380 | 850    | 0        | 1,100 | 800 | 4,650 |
| 3     | 1,380 | 850    | 1,200    | 1,100 | 800 | 5,850 |
| 4     | 1,580 | 1,050  | 1,200    | 1,100 | 1,000 | 6,850 |
| 5     | 1,580 | 1,050  | 1,200    | 2,800 | 1,000 | 9,850 |
| 6     | 1,580 | 1,050  | 1,200    | 3,300 | 1,800 | 11,450 |

### Pipeline Complexity

**Month 2 (Current):**
- Passes: 4 (Lex, Parse, IR Gen, Code Gen)
- Optimization: Basic (4 passes, no SSA)
- Analysis: Syntactic only
- Backends: 1 (C++)

**Month 6 (Final):**
- Passes: 6 (Lex, Parse, Semantic, IR Gen, Optimize, Code Gen)
- Optimization: Advanced (15+ passes, SSA-based)
- Analysis: Syntactic + Semantic + Dataflow
- Backends: 2-3 (C++, optional LLVM, extensible)

---

## 🔄 Data Flow Visualization

### Month 2 (Disconnected Pipeline)
```
Source ──Lex──> Tokens ──Parse──> AST ──┬──> IR ──Optimize──> [Unused]
                                         │
                                         └──> C++ Output
```

### Month 6 (Unified Pipeline)
```
Source ──Lex──> Tokens ──Parse──> AST ──Semantic──> Typed AST
                                                         │
                                                         ▼
                                              IR Gen ──> SSA IR
                                                         │
                                                         ▼
                                              Optimize ──> Optimized IR
                                                         │
                                                         ▼
                                              Register Alloc
                                                         │
                                                         ▼
                                              Backend Selection
                                                    /        \
                                                   ▼          ▼
                                              C++ Gen      LLVM Gen
                                                   │          │
                                                   ▼          ▼
                                               C++ Files   LLVM IR
```

---

## 🧩 Component Dependencies

### Month 2
```
RegexEngine ──> Lexer ──> Parser ──> CodeGen
                                │
                                └──> IRGen ──> IROptimizer
```

### Month 6
```
RegexEngine ──> Lexer ──> Parser ──> Semantic ──> IRGen ──> SSA
                                                      │
                                                      ▼
                                                  Optimizer ──> RegAlloc
                                                      │           │
                                                      ▼           ▼
                                                  CPPGen ←─> LLVMGen
                                                  
Dependencies:
    Semantic → SymbolTable, TypeSystem
    Optimizer → Dataflow, LoopAnalysis
    RegAlloc → LivenessAnalysis
```

---

## 📈 Capability Growth Chart

```
Capability                Month 2   Month 3   Month 4   Month 5   Month 6
────────────────────────────────────────────────────────────────────────────
Lexical Analysis          ████████  ████████  ████████  ████████  ████████ 100%
Syntax Analysis           ████████  ████████  ████████  ████████  ████████ 100%
Semantic Analysis         ░░░░░░░░  ███████░  ████████  ████████  ████████  0→100%
IR Generation             ███████░  ████████  ████████  ████████  ████████  70→100%
Optimization              ███░░░░░  ████░░░░  █████░░░  ████████  ████████  30→95%
Code Generation           ███████░  ████████  ████████  ████████  ████████  75→100%
Language Features         ██████░░  ██████░░  ████████  █████████ █████████  60→95%
────────────────────────────────────────────────────────────────────────────
Overall Completion        ████░░░░  █████░░░  ██████░░  ████████  █████████  40→90%
```

---

## 🎯 Key Architectural Decisions

### Decision 1: Two-Phase IR (Month 2→5)
**Problem**: Direct AST→C++ misses optimization opportunities  
**Solution**: AST → Three-address IR → SSA IR → C++  
**Benefit**: Clean separation, enables advanced optimizations

### Decision 2: Visitor Pattern Throughout
**Pattern**: All AST traversals use visitor pattern  
**Components**: Parser → Semantic → IRGen → CodeGen  
**Benefit**: Easy to add new passes without modifying AST

### Decision 3: Modular Backend (Month 6)
**Design**: Backend interface with multiple implementations  
**Implementations**: C++Backend, LLVMBackend, (future: Native)  
**Benefit**: Extensible to new targets without core changes

### Decision 4: Symbol Table Integration (Month 3)
**Integration Point**: Between Parser and IR Generator  
**Purpose**: Type information needed for both IR and codegen  
**Data Flow**: Parser → Semantic+SymbolTable → IR (with types)

---

## 🚀 Path Forward

**Current Achievement (Month 2)**:
- ✅ Solid foundation with 4,650 LOC
- ✅ Two parallel pipelines (AST→C++, AST→IR→Optimize)
- ✅ 24 tests, all passing
- ✅ 10 working examples

**Next Steps (Month 3-6)**:
1. **Connect the dots**: Integrate IR optimization into C++ generation
2. **Add intelligence**: Semantic analysis for correctness
3. **Maximize performance**: SSA + advanced optimizations
4. **Expand features**: Constructors, properties, delegates
5. **Multiple backends**: C++, LLVM, future-proof architecture

**Final Vision**: A production-quality, academically rigorous compiler demonstrating:
- Complete compilation theory (lex, parse, semantic, optimize, codegen)
- Advanced optimization techniques (SSA, dataflow, loop opts)
- Multiple backend targets (extensible architecture)
- ~11,000 LOC of clean, tested, documented code
- Research-grade foundation for further study

---

**Last Updated**: March 6, 2026  
**Current Architecture**: Month 2 (Completed)  
**Next Milestone**: Month 3 - Semantic Analysis Integration
