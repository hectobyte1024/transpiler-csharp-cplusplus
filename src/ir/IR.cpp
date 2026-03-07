#include "IR.h"
#include <sstream>

namespace Compiler {
namespace IR {

std::string opCodeToString(OpCode op) {
    switch (op) {
        case OpCode::Add: return "add";
        case OpCode::Sub: return "sub";
        case OpCode::Mul: return "mul";
        case OpCode::Div: return "div";
        case OpCode::Mod: return "mod";
        case OpCode::Eq: return "eq";
        case OpCode::Ne: return "ne";
        case OpCode::Lt: return "lt";
        case OpCode::Le: return "le";
        case OpCode::Gt: return "gt";
        case OpCode::Ge: return "ge";
        case OpCode::And: return "and";
        case OpCode::Or: return "or";
        case OpCode::Not: return "not";
        case OpCode::Load: return "load";
        case OpCode::Store: return "store";
        case OpCode::Alloca: return "alloca";
        case OpCode::Br: return "br";
        case OpCode::CondBr: return "condbr";
        case OpCode::Ret: return "ret";
        case OpCode::Call: return "call";
        case OpCode::Cast: return "cast";
        case OpCode::Label: return "label";
        case OpCode::Phi: return "phi";
        default: return "unknown";
    }
}

std::string Operand::toString() const {
    switch (type) {
        case Type::Temporary:
        case Type::Variable:
        case Type::Label:
            return name;
        case Type::Constant:
            if (std::holds_alternative<int>(value)) {
                return std::to_string(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                return std::to_string(std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value) ? "true" : "false";
            } else if (std::holds_alternative<std::string>(value)) {
                return "\"" + std::get<std::string>(value) + "\"";
            }
            return "constant";
    }
    return "";
}

std::string Instruction::toString() const {
    std::stringstream ss;
    
    if (opcode == OpCode::Label) {
        ss << label << ":";
        return ss.str();
    }
    
    ss << "  ";
    
    if (dest) {
        ss << dest->toString() << " = ";
    }
    
    ss << opCodeToString(opcode);
    
    if (src1) {
        ss << " " << src1->toString();
    }
    
    if (src2) {
        ss << ", " << src2->toString();
    }
    
    if (!label.empty() && opcode != OpCode::Label) {
        ss << " " << label;
    }
    
    return ss.str();
}

std::string BasicBlock::toString() const {
    std::stringstream ss;
    ss << name << ":\n";
    for (const auto& inst : instructions) {
        ss << inst->toString() << "\n";
    }
    return ss.str();
}

std::string Function::toString() const {
    std::stringstream ss;
    ss << "function " << returnType << " @" << name << "(";
    
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) ss << ", ";
        ss << parameters[i].first << " %" << parameters[i].second;
    }
    
    ss << ") {\n";
    
    for (const auto& block : blocks) {
        ss << block->toString();
    }
    
    ss << "}\n";
    return ss.str();
}

std::string Module::toString() const {
    std::stringstream ss;
    ss << "; Module: " << name << "\n\n";
    
    // Global variables
    for (const auto& gv : globalVariables) {
        ss << "@" << gv.second << " = global " << gv.first << "\n";
    }
    
    if (!globalVariables.empty()) {
        ss << "\n";
    }
    
    // Functions
    for (const auto& func : functions) {
        ss << func->toString() << "\n";
    }
    
    return ss.str();
}

} // namespace IR
} // namespace Compiler
