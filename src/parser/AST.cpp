#include "AST.h"

namespace Compiler {

// Expression accept implementations
void BinaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void LiteralExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void IdentifierExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void CallExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void MemberExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// Statement accept implementations
void ExpressionStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void VariableDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BlockStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void IfStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void WhileStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ForStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ClassDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void NamespaceDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UsingDirective::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void InterfaceDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void TryStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ThrowStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UsingStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Program::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace Compiler
