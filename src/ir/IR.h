#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace Compiler {
namespace IR {

// Three-Address Code Intermediate Representation
// Inspired by LLVM IR but simplified for educational purposes

// Forward declarations
class Instruction;
class BasicBlock;
class Function;
class Module;

// Value types
using Value = std::variant<int, double, std::string, bool>;

// Operand (can be temporary, constant, or variable)
struct Operand {
    enum class Type { Temporary, Variable, Constant, Label };
    Type type;
    std::string name;
    Value value;
    
    Operand(Type t, const std::string& n) : type(t), name(n) {}
    Operand(Type t, const std::string& n, Value v) : type(t), name(n), value(v) {}
    
    std::string toString() const;
};

// IR Instructions
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
    Br, CondBr, Ret, Call,
    // Type operations
    Cast,
    // Special
    Label, Phi
};

std::string opCodeToString(OpCode op);

class Instruction {
public:
    OpCode opcode;
    std::unique_ptr<Operand> dest;
    std::unique_ptr<Operand> src1;
    std::unique_ptr<Operand> src2;
    std::string label;  // For labels and branch targets
    
    Instruction(OpCode op) : opcode(op) {}
    
    Instruction(OpCode op, std::unique_ptr<Operand> d, 
                std::unique_ptr<Operand> s1, std::unique_ptr<Operand> s2 = nullptr)
        : opcode(op), dest(std::move(d)), src1(std::move(s1)), src2(std::move(s2)) {}
    
    std::string toString() const;
};

class BasicBlock {
public:
    std::string name;
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::vector<BasicBlock*> predecessors;
    std::vector<BasicBlock*> successors;
    
    BasicBlock(const std::string& n) : name(n) {}
    
    void addInstruction(std::unique_ptr<Instruction> inst) {
        instructions.push_back(std::move(inst));
    }
    
    void addPredecessor(BasicBlock* bb) {
        predecessors.push_back(bb);
    }
    
    void addSuccessor(BasicBlock* bb) {
        successors.push_back(bb);
    }
    
    std::string toString() const;
};

class Function {
public:
    std::string name;
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters;  // (type, name)
    std::vector<std::unique_ptr<BasicBlock>> blocks;
    int tempCounter;
    
    Function(const std::string& n, const std::string& ret) 
        : name(n), returnType(ret), tempCounter(0) {}
    
    std::string newTemp() {
        return "%t" + std::to_string(tempCounter++);
    }
    
    BasicBlock* createBlock(const std::string& name) {
        blocks.push_back(std::make_unique<BasicBlock>(name));
        return blocks.back().get();
    }
    
    std::string toString() const;
};

class Module {
public:
    std::string name;
    std::vector<std::unique_ptr<Function>> functions;
    std::vector<std::pair<std::string, std::string>> globalVariables;  // (type, name)
    
    Module(const std::string& n) : name(n) {}
    
    Function* createFunction(const std::string& name, const std::string& returnType) {
        functions.push_back(std::make_unique<Function>(name, returnType));
        return functions.back().get();
    }
    
    void addGlobalVariable(const std::string& type, const std::string& name) {
        globalVariables.push_back({type, name});
    }
    
    std::string toString() const;
};

} // namespace IR
} // namespace Compiler

#endif // IR_H
