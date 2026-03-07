#include "IROptimizer.h"
#include <algorithm>

namespace Compiler {
namespace IR {

void IROptimizer::optimize(int maxIterations) {
    bool changed = true;
    int iteration = 0;
    
    while (changed && iteration < maxIterations) {
        changed = false;
        
        // Run optimization passes
        changed |= runConstantFolding();
        changed |= runCopyPropagation();
        changed |= runDeadCodeElimination();
        changed |= runCommonSubexpressionElimination();
        
        iteration++;
    }
}

bool IROptimizer::isConstant(const Operand* op) {
    return op && op->type == Operand::Type::Constant;
}

bool IROptimizer::evaluateConstantOp(OpCode opcode, const Operand* src1, 
                                     const Operand* src2, Value& result) {
    if (!isConstant(src1)) return false;
    if (src2 && !isConstant(src2)) return false;
    
    // Handle integer constants
    if (std::holds_alternative<int>(src1->value)) {
        int val1 = std::get<int>(src1->value);
        
        if (!src2) {
            // Unary operations
            if (opcode == OpCode::Not) {
                result = !val1;
                return true;
            }
            return false;
        }
        
        if (!std::holds_alternative<int>(src2->value)) return false;
        int val2 = std::get<int>(src2->value);
        
        // Binary operations
        switch (opcode) {
            case OpCode::Add: result = val1 + val2; return true;
            case OpCode::Sub: result = val1 - val2; return true;
            case OpCode::Mul: result = val1 * val2; return true;
            case OpCode::Div: if (val2 != 0) { result = val1 / val2; return true; } break;
            case OpCode::Mod: if (val2 != 0) { result = val1 % val2; return true; } break;
            case OpCode::Eq: result = (val1 == val2); return true;
            case OpCode::Ne: result = (val1 != val2); return true;
            case OpCode::Lt: result = (val1 < val2); return true;
            case OpCode::Le: result = (val1 <= val2); return true;
            case OpCode::Gt: result = (val1 > val2); return true;
            case OpCode::Ge: result = (val1 >= val2); return true;
            case OpCode::And: result = (val1 && val2); return true;
            case OpCode::Or: result = (val1 || val2); return true;
            default: break;
        }
    }
    
    return false;
}

bool IROptimizer::runConstantFolding() {
    bool changed = false;
    
    for (auto& func : module->functions) {
        changed |= constantFold(func.get());
    }
    
    return changed;
}

bool IROptimizer::constantFold(Function* func) {
    bool changed = false;
    
    for (auto& block : func->blocks) {
        for (size_t i = 0; i < block->instructions.size(); i++) {
            auto& inst = block->instructions[i];
            
            // Skip non-arithmetic operations
            if (inst->opcode == OpCode::Label || inst->opcode == OpCode::Br ||
                inst->opcode == OpCode::CondBr || inst->opcode == OpCode::Ret ||
                inst->opcode == OpCode::Call) {
                continue;
            }
            
            // Try to evaluate constant operation
            Value result;
            if (evaluateConstantOp(inst->opcode, inst->src1.get(), inst->src2.get(), result)) {
                // Replace with load of constant
                auto newInst = std::make_unique<Instruction>(
                    OpCode::Load,
                    std::move(inst->dest),
                    std::make_unique<Operand>(Operand::Type::Constant, "", result)
                );
                inst = std::move(newInst);
                changed = true;
            }
        }
    }
    
    return changed;
}

bool IROptimizer::runDeadCodeElimination() {
    bool changed = false;
    
    for (auto& func : module->functions) {
        changed |= eliminateDeadCode(func.get());
    }
    
    return changed;
}

bool IROptimizer::eliminateDeadCode(Function* func) {
    bool changed = false;
    
    // Track which temporaries are used
    std::unordered_set<std::string> usedTemps;
    
    // First pass: mark all used temporaries
    for (auto& block : func->blocks) {
        for (auto& inst : block->instructions) {
            if (inst->src1 && inst->src1->type == Operand::Type::Temporary) {
                usedTemps.insert(inst->src1->name);
            }
            if (inst->src2 && inst->src2->type == Operand::Type::Temporary) {
                usedTemps.insert(inst->src2->name);
            }
        }
    }
    
    // Second pass: remove instructions that define unused temporaries
    for (auto& block : func->blocks) {
        auto& instructions = block->instructions;
        auto it = instructions.begin();
        
        while (it != instructions.end()) {
            auto& inst = *it;
            
            // Don't remove instructions with side effects
            if (inst->opcode == OpCode::Call || inst->opcode == OpCode::Store ||
                inst->opcode == OpCode::Ret || inst->opcode == OpCode::Br ||
                inst->opcode == OpCode::CondBr || inst->opcode == OpCode::Label) {
                ++it;
                continue;
            }
            
            // Remove if destination is unused temporary
            if (inst->dest && inst->dest->type == Operand::Type::Temporary &&
                usedTemps.find(inst->dest->name) == usedTemps.end()) {
                it = instructions.erase(it);
                changed = true;
            } else {
                ++it;
            }
        }
    }
    
    return changed;
}

bool IROptimizer::runCopyPropagation() {
    bool changed = false;
    
    for (auto& func : module->functions) {
        changed |= propagateCopies(func.get());
    }
    
    return changed;
}

bool IROptimizer::propagateCopies(Function* func) {
    bool changed = false;
    
    // Map of temp -> replacement
    std::unordered_map<std::string, std::string> copies;
    
    for (auto& block : func->blocks) {
        for (auto& inst : block->instructions) {
            // Detect copy: %t1 = load %t2
            if (inst->opcode == OpCode::Load && 
                inst->dest && inst->dest->type == Operand::Type::Temporary &&
                inst->src1 && inst->src1->type == Operand::Type::Temporary) {
                copies[inst->dest->name] = inst->src1->name;
            }
            
            // Replace uses of copies
            if (inst->src1 && inst->src1->type == Operand::Type::Temporary) {
                auto it = copies.find(inst->src1->name);
                if (it != copies.end()) {
                    inst->src1->name = it->second;
                    changed = true;
                }
            }
            
            if (inst->src2 && inst->src2->type == Operand::Type::Temporary) {
                auto it = copies.find(inst->src2->name);
                if (it != copies.end()) {
                    inst->src2->name = it->second;
                    changed = true;
                }
            }
        }
    }
    
    return changed;
}

bool IROptimizer::runCommonSubexpressionElimination() {
    bool changed = false;
    
    for (auto& func : module->functions) {
        changed |= eliminateCommonSubexpressions(func.get());
    }
    
    return changed;
}

bool IROptimizer::eliminateCommonSubexpressions(Function* func) {
    bool changed = false;
    
    // Map of (opcode, src1, src2) -> dest temp
    std::unordered_map<std::string, std::string> expressions;
    
    for (auto& block : func->blocks) {
        for (auto& inst : block->instructions) {
            // Only handle pure operations
            if (inst->opcode != OpCode::Add && inst->opcode != OpCode::Sub &&
                inst->opcode != OpCode::Mul && inst->opcode != OpCode::Div &&
                inst->opcode != OpCode::And && inst->opcode != OpCode::Or) {
                continue;
            }
            
            // Build expression key
            std::string key = opCodeToString(inst->opcode);
            if (inst->src1) key += "," + inst->src1->toString();
            if (inst->src2) key += "," + inst->src2->toString();
            
            auto it = expressions.find(key);
            if (it != expressions.end() && inst->dest) {
                // Found common subexpression - replace with load
                auto temp = it->second;
                auto newInst = std::make_unique<Instruction>(
                    OpCode::Load,
                    std::move(inst->dest),
                    std::make_unique<Operand>(Operand::Type::Temporary, temp)
                );
                inst = std::move(newInst);
                changed = true;
            } else if (inst->dest) {
                // Remember this expression
                expressions[key] = inst->dest->name;
            }
        }
    }
    
    return changed;
}

} // namespace IR
} // namespace Compiler
