#ifndef IR_OPTIMIZER_H
#define IR_OPTIMIZER_H

#include "IR.h"
#include <unordered_map>
#include <unordered_set>

namespace Compiler {
namespace IR {

// IR Optimization Passes
class IROptimizer {
private:
    Module* module;
    
    // Constant folding
    bool constantFold(Function* func);
    
    // Dead code elimination
    bool eliminateDeadCode(Function* func);
    
    // Common subexpression elimination
    bool eliminateCommonSubexpressions(Function* func);
    
    // Copy propagation
    bool propagateCopies(Function* func);
    
    // Helper methods
    bool isConstant(const Operand* op);
    bool evaluateConstantOp(OpCode opcode, const Operand* src1, const Operand* src2, Value& result);
    void markLiveInstructions(BasicBlock* block, std::unordered_set<Instruction*>& live);
    
public:
    IROptimizer(Module* mod) : module(mod) {}
    
    // Run all optimization passes
    void optimize(int maxIterations = 3);
    
    // Individual optimization passes
    bool runConstantFolding();
    bool runDeadCodeElimination();
    bool runCopyPropagation();
    bool runCommonSubexpressionElimination();
};

} // namespace IR
} // namespace Compiler

#endif // IR_OPTIMIZER_H
