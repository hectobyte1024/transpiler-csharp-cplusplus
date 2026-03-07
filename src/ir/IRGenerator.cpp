#include "IRGenerator.h"

namespace Compiler {

IRGenerator::IRGenerator() : module(nullptr), currentFunction(nullptr), 
                             currentBlock(nullptr), labelCounter(0) {}

std::unique_ptr<IR::Module> IRGenerator::generate(Program& program) {
    auto mod = std::make_unique<IR::Module>("program");
    module = mod.get();
    
    program.accept(*this);
    
    return mod;
}

void IRGenerator::visit(BinaryExpression& node) {
    // Generate code for left operand
    node.left->accept(*this);
    std::string leftTemp = lastTemp;
    
    // Generate code for right operand
    node.right->accept(*this);
    std::string rightTemp = lastTemp;
    
    // Create result temporary
    std::string resultTemp = currentFunction->newTemp();
    
    // Map C# operators to IR opcodes
    IR::OpCode opcode;
    if (node.op == "+") opcode = IR::OpCode::Add;
    else if (node.op == "-") opcode = IR::OpCode::Sub;
    else if (node.op == "*") opcode = IR::OpCode::Mul;
    else if (node.op == "/") opcode = IR::OpCode::Div;
    else if (node.op == "%") opcode = IR::OpCode::Mod;
    else if (node.op == "==") opcode = IR::OpCode::Eq;
    else if (node.op == "!=") opcode = IR::OpCode::Ne;
    else if (node.op == "<") opcode = IR::OpCode::Lt;
    else if (node.op == "<=") opcode = IR::OpCode::Le;
    else if (node.op == ">") opcode = IR::OpCode::Gt;
    else if (node.op == ">=") opcode = IR::OpCode::Ge;
    else if (node.op == "&&") opcode = IR::OpCode::And;
    else if (node.op == "||") opcode = IR::OpCode::Or;
    else opcode = IR::OpCode::Add;  // Default
    
    auto inst = std::make_unique<IR::Instruction>(
        opcode,
        std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, resultTemp),
        std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, leftTemp),
        std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, rightTemp)
    );
    
    currentBlock->addInstruction(std::move(inst));
    lastTemp = resultTemp;
}

void IRGenerator::visit(UnaryExpression& node) {
    node.operand->accept(*this);
    std::string operandTemp = lastTemp;
    
    std::string resultTemp = currentFunction->newTemp();
    
    IR::OpCode opcode;
    if (node.op == "-") {
        // Unary minus: 0 - operand
        auto zero = std::make_unique<IR::Operand>(IR::Operand::Type::Constant, "0", 0);
        auto inst = std::make_unique<IR::Instruction>(
            IR::OpCode::Sub,
            std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, resultTemp),
            std::move(zero),
            std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, operandTemp)
        );
        currentBlock->addInstruction(std::move(inst));
    } else if (node.op == "!") {
        opcode = IR::OpCode::Not;
        auto inst = std::make_unique<IR::Instruction>(
            opcode,
            std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, resultTemp),
            std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, operandTemp)
        );
        currentBlock->addInstruction(std::move(inst));
    }
    
    lastTemp = resultTemp;
}

void IRGenerator::visit(LiteralExpression& node) {
    std::string temp = currentFunction->newTemp();
    
    IR::Value value;
    if (node.type == "int") {
        value = std::stoi(node.value);
    } else if (node.type == "double") {
        value = std::stod(node.value);
    } else if (node.type == "bool") {
        value = (node.value == "true");
    } else {
        value = node.value;
    }
    
    auto inst = std::make_unique<IR::Instruction>(
        IR::OpCode::Load,
        std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, temp),
        std::make_unique<IR::Operand>(IR::Operand::Type::Constant, node.value, value)
    );
    
    currentBlock->addInstruction(std::move(inst));
    lastTemp = temp;
}

void IRGenerator::visit(IdentifierExpression& node) {
    std::string temp = currentFunction->newTemp();
    
    auto inst = std::make_unique<IR::Instruction>(
        IR::OpCode::Load,
        std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, temp),
        std::make_unique<IR::Operand>(IR::Operand::Type::Variable, node.name)
    );
    
    currentBlock->addInstruction(std::move(inst));
    lastTemp = temp;
}

void IRGenerator::visit(CallExpression& node) {
    // Simplified call handling
    std::string temp = currentFunction->newTemp();
    
    auto inst = std::make_unique<IR::Instruction>(IR::OpCode::Call);
    inst->dest = std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, temp);
    inst->label = "<function>";  // Simplified - would need to extract function name
    
    currentBlock->addInstruction(std::move(inst));
    lastTemp = temp;
}

void IRGenerator::visit(MemberExpression& node) {
    // Simplified member access
    node.object->accept(*this);
    std::string objTemp = lastTemp;
    
    std::string temp = currentFunction->newTemp();
    auto inst = std::make_unique<IR::Instruction>(
        IR::OpCode::Load,
        std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, temp),
        std::make_unique<IR::Operand>(IR::Operand::Type::Variable, objTemp + "." + node.member)
    );
    
    currentBlock->addInstruction(std::move(inst));
    lastTemp = temp;
}

void IRGenerator::visit(ExpressionStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void IRGenerator::visit(VariableDeclaration& node) {
    // Allocate space
    auto allocaInst = std::make_unique<IR::Instruction>(
        IR::OpCode::Alloca,
        std::make_unique<IR::Operand>(IR::Operand::Type::Variable, node.name),
        std::make_unique<IR::Operand>(IR::Operand::Type::Constant, node.type, node.type)
    );
    currentBlock->addInstruction(std::move(allocaInst));
    
    // Initialize if there's an initializer
    if (node.initializer) {
        node.initializer->accept(*this);
        std::string initTemp = lastTemp;
        
        auto storeInst = std::make_unique<IR::Instruction>(
            IR::OpCode::Store,
            std::make_unique<IR::Operand>(IR::Operand::Type::Variable, node.name),
            std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, initTemp)
        );
        currentBlock->addInstruction(std::move(storeInst));
    }
}

void IRGenerator::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void IRGenerator::visit(IfStatement& node) {
    std::string thenLabel = newLabel();
    std::string elseLabel = newLabel();
    std::string endLabel = newLabel();
    
    // Condition
    node.condition->accept(*this);
    std::string condTemp = lastTemp;
    
    // Conditional branch
    auto condBr = std::make_unique<IR::Instruction>(IR::OpCode::CondBr);
    condBr->src1 = std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, condTemp);
    condBr->label = thenLabel + " " + (node.elseBranch ? elseLabel : endLabel);
    currentBlock->addInstruction(std::move(condBr));
    
    // Then block
    auto thenBlock = currentFunction->createBlock(thenLabel);
    currentBlock = thenBlock;
    node.thenBranch->accept(*this);
    auto brEnd = std::make_unique<IR::Instruction>(IR::OpCode::Br);
    brEnd->label = endLabel;
    currentBlock->addInstruction(std::move(brEnd));
    
    // Else block
    if (node.elseBranch) {
        auto elseBlock = currentFunction->createBlock(elseLabel);
        currentBlock = elseBlock;
        node.elseBranch->accept(*this);
        auto brEnd2 = std::make_unique<IR::Instruction>(IR::OpCode::Br);
        brEnd2->label = endLabel;
        currentBlock->addInstruction(std::move(brEnd2));
    }
    
    // End block
    auto endBlock = currentFunction->createBlock(endLabel);
    currentBlock = endBlock;
}

void IRGenerator::visit(WhileStatement& node) {
    std::string condLabel = newLabel();
    std::string bodyLabel = newLabel();
    std::string endLabel = newLabel();
    
    // Jump to condition
    auto brCond = std::make_unique<IR::Instruction>(IR::OpCode::Br);
    brCond->label = condLabel;
    currentBlock->addInstruction(std::move(brCond));
    
    // Condition block
    auto condBlock = currentFunction->createBlock(condLabel);
    currentBlock = condBlock;
    node.condition->accept(*this);
    std::string condTemp = lastTemp;
    
    auto condBr = std::make_unique<IR::Instruction>(IR::OpCode::CondBr);
    condBr->src1 = std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, condTemp);
    condBr->label = bodyLabel + " " + endLabel;
    currentBlock->addInstruction(std::move(condBr));
    
    // Body block
    auto bodyBlock = currentFunction->createBlock(bodyLabel);
    currentBlock = bodyBlock;
    node.body->accept(*this);
    auto brCondAgain = std::make_unique<IR::Instruction>(IR::OpCode::Br);
    brCondAgain->label = condLabel;
    currentBlock->addInstruction(std::move(brCondAgain));
    
    // End block
    auto endBlock = currentFunction->createBlock(endLabel);
    currentBlock = endBlock;
}

void IRGenerator::visit(ForStatement& node) {
    // For loops are similar to while, just with init and update
    if (node.initializer) {
        node.initializer->accept(*this);
    }
    
    std::string condLabel = newLabel();
    std::string bodyLabel = newLabel();
    std::string updateLabel = newLabel();
    std::string endLabel = newLabel();
    
    auto brCond = std::make_unique<IR::Instruction>(IR::OpCode::Br);
    brCond->label = condLabel;
    currentBlock->addInstruction(std::move(brCond));
    
    // Condition
    auto condBlock = currentFunction->createBlock(condLabel);
    currentBlock = condBlock;
    if (node.condition) {
        node.condition->accept(*this);
        std::string condTemp = lastTemp;
        
        auto condBr = std::make_unique<IR::Instruction>(IR::OpCode::CondBr);
        condBr->src1 = std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, condTemp);
        condBr->label = bodyLabel + " " + endLabel;
        currentBlock->addInstruction(std::move(condBr));
    }
    
    // Body
    auto bodyBlock = currentFunction->createBlock(bodyLabel);
    currentBlock = bodyBlock;
    node.body->accept(*this);
    
    // Update
    auto updateBlock = currentFunction->createBlock(updateLabel);
    currentBlock = updateBlock;
    if (node.increment) {
        node.increment->accept(*this);
    }
    auto brCondAgain = std::make_unique<IR::Instruction>(IR::OpCode::Br);
    brCondAgain->label = condLabel;
    currentBlock->addInstruction(std::move(brCondAgain));
    
    // End
    auto endBlock = currentFunction->createBlock(endLabel);
    currentBlock = endBlock;
}

void IRGenerator::visit(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
        auto retInst = std::make_unique<IR::Instruction>(
            IR::OpCode::Ret,
            nullptr,
            std::make_unique<IR::Operand>(IR::Operand::Type::Temporary, lastTemp)
        );
        currentBlock->addInstruction(std::move(retInst));
    } else {
        auto retInst = std::make_unique<IR::Instruction>(IR::OpCode::Ret);
        currentBlock->addInstruction(std::move(retInst));
    }
}

void IRGenerator::visit(FunctionDeclaration& node) {
    auto func = module->createFunction(node.name, node.returnType);
    currentFunction = func;
    
    // Add parameters
    for (const auto& param : node.parameters) {
        func->parameters.push_back(param);
    }
    
    // Create entry block
    auto entryBlock = func->createBlock("entry");
    currentBlock = entryBlock;
    
    // Generate body
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Ensure there's a return
    if (currentBlock->instructions.empty() || 
        currentBlock->instructions.back()->opcode != IR::OpCode::Ret) {
        auto retInst = std::make_unique<IR::Instruction>(IR::OpCode::Ret);
        currentBlock->addInstruction(std::move(retInst));
    }
}

void IRGenerator::visit(ClassDeclaration& node) {
    // Classes don't directly translate to IR - we process their methods
    for (auto& method : node.methods) {
        method->accept(*this);
    }
}

void IRGenerator::visit(NamespaceDeclaration& node) {
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
}

void IRGenerator::visit(UsingDirective& node) {
    // Using directives don't affect IR
}

void IRGenerator::visit(InterfaceDeclaration& node) {
    // Interfaces don't generate IR
}

void IRGenerator::visit(TryStatement& node) {
    // Simplified exception handling in IR
    if (node.tryBlock) {
        node.tryBlock->accept(*this);
    }
    // Catch/finally handling would require more complex CFG
}

void IRGenerator::visit(ThrowStatement& node) {
    // Simplified throw
    auto throwInst = std::make_unique<IR::Instruction>(IR::OpCode::Call);
    throwInst->label = "throw";
    currentBlock->addInstruction(std::move(throwInst));
}

void IRGenerator::visit(UsingStatement& node) {
    if (node.resource) {
        node.resource->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void IRGenerator::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

} // namespace Compiler
