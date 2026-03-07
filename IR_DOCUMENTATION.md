# Intermediate Representation (IR) Documentation

## Overview

The C# to C++ compiler uses a three-address code intermediate representation inspired by LLVM IR. This provides a clean separation between parsing (AST construction) and code generation, enabling optimization passes and future backend support.

## IR Structure

### Module
The top-level IR object representing a compilation unit.

```
Module
  └── Function[]
        └── BasicBlock[]
              └── Instruction[]
```

### Function
Represents a C# method or function.

**Properties:**
- `name`: Function name (e.g., `@Main`)
- `returnType`: Return type as string
- `parameters`: List of parameters
- `blocks`: List of basic blocks
- `tempCounter`: Counter for generating temporary variables

**Example:**
```
function void @Main() {
  entry:
    x = alloca "int"
    %t0 = load 42
    x = store %t0
    ret
}
```

### BasicBlock
A sequence of instructions with a single entry and exit point.

**Properties:**
- `label`: Unique identifier (e.g., `entry`, `L0`, `L1`)
- `instructions`: List of instructions

**Invariants:**
- First instruction is typically a label
- Last instruction is typically a terminator (br, condbr, ret)

### Instruction
A single IR operation in three-address code format.

**Format:** `result = opcode operand1, operand2`

**Components:**
- `opcode`: Operation type (OpCode enum)
- `result`: Destination operand (usually a temporary)
- `operands`: Source operands (0-2 depending on operation)
- `label`: Used for control flow (branches, labels, calls)

## OpCode Reference

### Arithmetic Operations
| OpCode | Format | Description | Example |
|--------|--------|-------------|---------|
| `Add` | `%t = add %a, %b` | Integer addition | `%t0 = add %t1, %t2` |
| `Sub` | `%t = sub %a, %b` | Integer subtraction | `%t0 = sub %t1, %t2` |
| `Mul` | `%t = mul %a, %b` | Integer multiplication | `%t0 = mul %t1, %t2` |
| `Div` | `%t = div %a, %b` | Integer division | `%t0 = div %t1, %t2` |
| `Mod` | `%t = mod %a, %b` | Modulo operation | `%t0 = mod %t1, %t2` |

### Comparison Operations
| OpCode | Format | Description | Example |
|--------|--------|-------------|---------|
| `Eq` | `%t = eq %a, %b` | Equality comparison | `%t0 = eq %t1, %t2` |
| `Ne` | `%t = ne %a, %b` | Not equal comparison | `%t0 = ne %t1, %t2` |
| `Lt` | `%t = lt %a, %b` | Less than | `%t0 = lt %t1, %t2` |
| `Le` | `%t = le %a, %b` | Less than or equal | `%t0 = le %t1, %t2` |
| `Gt` | `%t = gt %a, %b` | Greater than | `%t0 = gt %t1, %t2` |
| `Ge` | `%t = ge %a, %b` | Greater than or equal | `%t0 = ge %t1, %t2` |

### Logical Operations
| OpCode | Format | Description | Example |
|--------|--------|-------------|---------|
| `And` | `%t = and %a, %b` | Logical AND | `%t0 = and %t1, %t2` |
| `Or` | `%t = or %a, %b` | Logical OR | `%t0 = or %t1, %t2` |
| `Not` | `%t = not %a` | Logical NOT | `%t0 = not %t1` |

### Memory Operations
| OpCode | Format | Description | Example |
|--------|--------|-------------|---------|
| `Load` | `%t = load source` | Load value into temporary | `%t0 = load x` |
| `Store` | `var = store %t` | Store temporary to variable | `x = store %t0` |
| `Alloca` | `var = alloca type` | Allocate variable | `x = alloca "int"` |

### Control Flow
| OpCode | Format | Description | Example |
|--------|--------|-------------|---------|
| `Br` | `br label` | Unconditional branch | `br L0` |
| `CondBr` | `condbr %cond label1 label2` | Conditional branch | `condbr %t0 L1 L2` |
| `Label` | `label:` | Basic block label | `L0:` |
| `Ret` | `ret` or `ret %value` | Return from function | `ret %t0` |

### Function Operations
| OpCode | Format | Description | Example |
|--------|--------|-------------|---------|
| `Call` | `%t = call function` | Function call | `%t0 = call @foo` |
| `Cast` | `%t = cast %src type` | Type conversion | `%t0 = cast %t1 "double"` |

### Special
| OpCode | Format | Description | Example |
|--------|--------|-------------|---------|
| `Phi` | `%t = phi [%v1, L1], [%v2, L2]` | SSA phi node (future use) | N/A |

## Operand Types

### Temporary
Compiler-generated variable with `%` prefix.
- **Format:** `%t0`, `%t1`, `%t2`, ...
- **Scope:** Local to function
- **Use:** Hold intermediate computation results

### Variable
User-declared or compiler-allocated variable.
- **Format:** `x`, `sum`, `myVar`
- **Scope:** Can span multiple basic blocks
- **Use:** Store program state

### Constant
Literal numeric value.
- **Format:** `42`, `3.14`, `true`
- **Type:** Integer, float, or boolean
- **Use:** Constant folding optimization

### Label
Basic block identifier.
- **Format:** `L0`, `L1`, `entry`, `end`
- **Use:** Control flow target

## IR Generation Examples

### Variable Declaration and Assignment
**C# Code:**
```csharp
int x = 42;
```

**Generated IR:**
```
x = alloca "int"
%t0 = load 42
x = store %t0
```

### Binary Expression
**C# Code:**
```csharp
int result = a + b * 2;
```

**Generated IR:**
```
%t0 = load a
%t1 = load b
%t2 = load 2
%t3 = mul %t1, %t2
%t4 = add %t0, %t3
result = alloca "int"
result = store %t4
```

### If Statement
**C# Code:**
```csharp
if (x > 10) {
    y = 1;
} else {
    y = 2;
}
```

**Generated IR:**
```
%t0 = load x
%t1 = load 10
%t2 = gt %t0, %t1
condbr %t2 L0 L1
L0:
  y = alloca "int"
  %t3 = load 1
  y = store %t3
  br L2
L1:
  y = alloca "int"
  %t4 = load 2
  y = store %t4
  br L2
L2:
```

### While Loop
**C# Code:**
```csharp
while (i < n) {
    sum = sum + i;
    i = i + 1;
}
```

**Generated IR:**
```
br L0
L0:
  %t0 = load i
  %t1 = load n
  %t2 = lt %t0, %t1
  condbr %t2 L1 L2
L1:
  %t3 = load sum
  %t4 = load i
  %t5 = add %t3, %t4
  sum = store %t5
  %t6 = load i
  %t7 = load 1
  %t8 = add %t6, %t7
  i = store %t8
  br L0
L2:
```

### For Loop
**C# Code:**
```csharp
for (int i = 0; i < 10; i++) {
    sum = sum + i;
}
```

**Generated IR:**
```
i = alloca "int"
%t0 = load 0
i = store %t0
br L0
L0:
  %t1 = load i
  %t2 = load 10
  %t3 = lt %t1, %t2
  condbr %t3 L1 L3
L1:
  %t4 = load sum
  %t5 = load i
  %t6 = add %t4, %t5
  sum = store %t6
  br L2
L2:
  %t7 = load i
  %t8 = load 1
  %t9 = add %t7, %t8
  i = store %t9
  br L0
L3:
```

## Optimization Passes

### Constant Folding
Evaluates constant expressions at compile time.

**Before:**
```
%t0 = load 2
%t1 = load 3
%t2 = add %t0, %t1
```

**After:**
```
%t2 = load 5
```

**Benefits:**
- Reduced runtime computation
- Enables further optimizations

### Dead Code Elimination
Removes instructions that compute values never used.

**Before:**
```
%t0 = load 42      ; %t0 never used
%t1 = load x
ret %t1
```

**After:**
```
%t1 = load x
ret %t1
```

**Algorithm:**
1. Mark all instructions as dead
2. Mark instructions that affect program output (stores, returns, calls)
3. Recursively mark operands of live instructions
4. Remove unmarked instructions

### Copy Propagation
Replaces uses of a copy with the original value.

**Before:**
```
%t0 = load x
%t1 = load %t0
%t2 = add %t1, 5
```

**After:**
```
%t0 = load x
%t2 = add %t0, 5
```

**Benefits:**
- Fewer temporary variables
- Enables other optimizations

### Common Subexpression Elimination (CSE)
Reuses results of identical computations.

**Before:**
```
%t0 = load x
%t1 = load y
%t2 = add %t0, %t1
; ... later ...
%t3 = load x
%t4 = load y
%t5 = add %t3, %t4  ; Same as %t2
```

**After:**
```
%t0 = load x
%t1 = load y
%t2 = add %t0, %t1
; ... later ...
%t5 = load %t2  ; Reuse result
```

## IR Optimization Architecture

### Fixed-Point Iteration
The optimizer runs passes repeatedly until no changes occur or max iterations reached.

```cpp
void optimize(Module& module, int maxIterations) {
    for (int i = 0; i < maxIterations; i++) {
        bool changed = false;
        
        changed |= constantFold();
        changed |= eliminateDeadCode();
        changed |= propagateCopies();
        changed |= eliminateCommonSubexpressions();
        
        if (!changed) break;
    }
}
```

### Pass Ordering
1. **Constant Folding** - Creates more constants for other passes
2. **Dead Code Elimination** - Removes unused code
3. **Copy Propagation** - Simplifies value flow
4. **CSE** - Finds redundant computations

## Using IR in the Compiler

### Generating IR
```bash
./cs2cpp examples/hello.cs --emit-ir --verbose
```

This will:
1. Parse C# code to AST
2. Generate IR from AST
3. Display unoptimized IR
4. Write IR to `output.ir`

### Optimizing IR
```bash
./cs2cpp examples/hello.cs --emit-ir --optimize --opt-level=2 --verbose
```

This will:
1. Parse and generate IR
2. Run optimization passes (level 2 → 10 iterations)
3. Display optimized IR
4. Write optimized IR to `output.ir`

### Optimization Levels
- `--opt-level=1`: 5 iterations (minimal optimization)
- `--opt-level=2`: 10 iterations (default, balanced)
- `--opt-level=3`: 15 iterations (aggressive)

## Future Plans

### SSA Form
Convert IR to Static Single Assignment form:
- Add phi nodes at merge points
- Each variable assigned exactly once
- Enables more powerful optimizations

### Additional Optimizations
- Loop-invariant code motion
- Strength reduction
- Induction variable optimization
- Register allocation hints

### Backend Integration
- Translate optimized IR to C++ (currently AST → C++)
- Or generate native code via LLVM backend
- Or emit assembly directly

## Implementation Files

| File | Purpose |
|------|---------|
| `src/ir/IR.h` | IR data structure definitions |
| `src/ir/IR.cpp` | IR implementation and toString methods |
| `src/ir/IRGenerator.h` | AST to IR translator interface |
| `src/ir/IRGenerator.cpp` | IR generation from AST |
| `src/ir/IROptimizer.h` | Optimization pass framework |
| `src/ir/IROptimizer.cpp` | Optimization implementations |

## References

- **LLVM IR Reference**: https://llvm.org/docs/LangRef.html
- **Three-Address Code**: Aho et al., "Compilers: Principles, Techniques, and Tools"
- **SSA Form**: Cytron et al., "Efficiently Computing Static Single Assignment Form"
- **Dataflow Analysis**: Kildall, "A Unified Approach to Global Program Optimization"
