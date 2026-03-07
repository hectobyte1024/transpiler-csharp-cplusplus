#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

namespace Compiler {

// Forward declarations
class ASTVisitor;

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// Expression nodes
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(std::unique_ptr<Expression> left, const std::string& op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class UnaryExpression : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(const std::string& op, std::unique_ptr<Expression> operand)
        : op(op), operand(std::move(operand)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class LiteralExpression : public Expression {
public:
    std::string value;
    std::string type; // "int", "float", "string", "char", "bool"
    
    LiteralExpression(const std::string& value, const std::string& type)
        : value(value), type(type) {}
    
    void accept(ASTVisitor& visitor) override;
};

class IdentifierExpression : public Expression {
public:
    std::string name;
    
    IdentifierExpression(const std::string& name) : name(name) {}
    
    void accept(ASTVisitor& visitor) override;
};

class CallExpression : public Expression {
public:
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    CallExpression(std::unique_ptr<Expression> callee)
        : callee(std::move(callee)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class MemberExpression : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string member;
    
    MemberExpression(std::unique_ptr<Expression> object, const std::string& member)
        : object(std::move(object)), member(member) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Statement nodes
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class VariableDeclaration : public Statement {
public:
    std::string type;
    std::string name;
    std::unique_ptr<Expression> initializer;
    
    VariableDeclaration(const std::string& type, const std::string& name, 
                       std::unique_ptr<Expression> initializer = nullptr)
        : type(type), name(name), initializer(std::move(initializer)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    void accept(ASTVisitor& visitor) override;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
    
    IfStatement(std::unique_ptr<Expression> condition,
                std::unique_ptr<Statement> thenBranch,
                std::unique_ptr<Statement> elseBranch = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    
    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class ForStatement : public Statement {
public:
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> increment;
    std::unique_ptr<Statement> body;
    
    ForStatement(std::unique_ptr<Statement> initializer,
                 std::unique_ptr<Expression> condition,
                 std::unique_ptr<Expression> increment,
                 std::unique_ptr<Statement> body)
        : initializer(std::move(initializer)), condition(std::move(condition)),
          increment(std::move(increment)), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;
    
    ReturnStatement(std::unique_ptr<Expression> value = nullptr)
        : value(std::move(value)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Declaration nodes
class FunctionDeclaration : public Statement {
public:
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters; // (type, name)
    std::unique_ptr<BlockStatement> body;
    std::vector<std::string> modifiers; // public, private, static, virtual, abstract, etc.
    bool isPureVirtual;  // For interface methods
    
    FunctionDeclaration(const std::string& returnType, const std::string& name)
        : returnType(returnType), name(name), isPureVirtual(false) {}
    
    void accept(ASTVisitor& visitor) override;
};

class ClassDeclaration : public Statement {
public:
    std::string name;
    std::string baseClass;  // For inheritance
    std::vector<std::string> interfaces;  // Implemented interfaces
    std::vector<std::string> templateParams;  // Generic parameters
    std::vector<std::unique_ptr<FunctionDeclaration>> methods;
    std::vector<std::unique_ptr<VariableDeclaration>> fields;
    bool isAbstract;
    
    ClassDeclaration(const std::string& name) 
        : name(name), isAbstract(false) {}
    
    void accept(ASTVisitor& visitor) override;
};

class NamespaceDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::unique_ptr<Statement>> declarations;
    
    NamespaceDeclaration(const std::string& name) : name(name) {}
    
    void accept(ASTVisitor& visitor) override;
};

class UsingDirective : public Statement {
public:
    std::string namespaceName;
    
    UsingDirective(const std::string& namespaceName) : namespaceName(namespaceName) {}
    
    void accept(ASTVisitor& visitor) override;
};

class InterfaceDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::unique_ptr<FunctionDeclaration>> methods;
    
    InterfaceDeclaration(const std::string& name) : name(name) {}
    
    void accept(ASTVisitor& visitor) override;
};

class TryStatement : public Statement {
public:
    std::unique_ptr<BlockStatement> tryBlock;
    std::vector<std::pair<std::string, std::unique_ptr<BlockStatement>>> catchBlocks; // (exceptionType, block)
    std::unique_ptr<BlockStatement> finallyBlock;
    
    TryStatement(std::unique_ptr<BlockStatement> tryBlock)
        : tryBlock(std::move(tryBlock)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class ThrowStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    ThrowStatement(std::unique_ptr<Expression> expression = nullptr)
        : expression(std::move(expression)) {}
    
    void accept(ASTVisitor& visitor) override;
};

class UsingStatement : public Statement {
public:
    std::unique_ptr<VariableDeclaration> resource;
    std::unique_ptr<Statement> body;
    
    UsingStatement(std::unique_ptr<VariableDeclaration> resource, std::unique_ptr<Statement> body)
        : resource(std::move(resource)), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Program (root node)
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    void accept(ASTVisitor& visitor) override;
};

// Visitor pattern for AST traversal
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(BinaryExpression& node) = 0;
    virtual void visit(UnaryExpression& node) = 0;
    virtual void visit(LiteralExpression& node) = 0;
    virtual void visit(IdentifierExpression& node) = 0;
    virtual void visit(CallExpression& node) = 0;
    virtual void visit(MemberExpression& node) = 0;
    
    virtual void visit(ExpressionStatement& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(BlockStatement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ForStatement& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    
    virtual void visit(FunctionDeclaration& node) = 0;
    virtual void visit(ClassDeclaration& node) = 0;
    virtual void visit(NamespaceDeclaration& node) = 0;
    virtual void visit(UsingDirective& node) = 0;
    virtual void visit(InterfaceDeclaration& node) = 0;
    virtual void visit(TryStatement& node) = 0;
    virtual void visit(ThrowStatement& node) = 0;
    virtual void visit(UsingStatement& node) = 0;
    
    virtual void visit(Program& node) = 0;
};

} // namespace Compiler

#endif // AST_H
