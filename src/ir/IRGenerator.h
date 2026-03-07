#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "../parser/AST.h"
#include "IR.h"
#include <stack>

namespace Compiler {

// Generates IR from AST
class IRGenerator : public ASTVisitor {
private:
    IR::Module* module;
    IR::Function* currentFunction;
    IR::BasicBlock* currentBlock;
    std::string lastTemp;  // Last temporary generated (for expression results)
    int labelCounter;
    
    std::string newLabel() {
        return "L" + std::to_string(labelCounter++);
    }
    
public:
    IRGenerator();
    
    std::unique_ptr<IR::Module> generate(Program& program);
    
    // Expression visitors - return the temporary holding the result
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(LiteralExpression& node) override;
    void visit(IdentifierExpression& node) override;
    void visit(CallExpression& node) override;
    void visit(MemberExpression& node) override;
    
    // Statement visitors
    void visit(ExpressionStatement& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ReturnStatement& node) override;
    
    // Declaration visitors
    void visit(FunctionDeclaration& node) override;
    void visit(ClassDeclaration& node) override;
    void visit(NamespaceDeclaration& node) override;
    void visit(UsingDirective& node) override;
    void visit(InterfaceDeclaration& node) override;
    void visit(TryStatement& node) override;
    void visit(ThrowStatement& node) override;
    void visit(UsingStatement& node) override;
    
    void visit(Program& node) override;
};

} // namespace Compiler

#endif // IR_GENERATOR_H
