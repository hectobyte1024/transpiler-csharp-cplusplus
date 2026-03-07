#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "../lexer/Token.h"
#include "../lexer/Lexer.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace Compiler {

// Parse error with location information
struct ParseError {
    std::string message;
    int line;
    int column;
    std::string context;
    
    ParseError(const std::string& msg, int l, int c, const std::string& ctx = "")
        : message(msg), line(l), column(c), context(ctx) {}
    
    std::string toString() const {
        return "Error at line " + std::to_string(line) + ":" + std::to_string(column) + 
               ": " + message;
    }
};

// Recursive Descent Parser with Error Recovery
class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    std::vector<ParseError> errors;  // Collected errors
    bool panicMode;  // Error recovery mode
    
    Token peek();
    Token peekNext();  // Lookahead by 1
    Token previous();
    Token advance();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    
    // Error recovery
    void reportError(const std::string& message);
    void synchronize();  // Recover from errors
    bool hasSynchronizationPoint();
    
    // Parsing methods
    std::unique_ptr<Program> program();
    
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<UsingDirective> usingDirective();
    std::unique_ptr<NamespaceDeclaration> namespaceDeclaration();
    std::unique_ptr<ClassDeclaration> classDeclaration();
    std::unique_ptr<InterfaceDeclaration> interfaceDeclaration();
    std::unique_ptr<FunctionDeclaration> functionDeclaration(const std::vector<std::string>& modifiers);
    std::unique_ptr<VariableDeclaration> variableDeclaration();
    
    std::unique_ptr<Statement> statement();
    std::unique_ptr<BlockStatement> blockStatement();
    std::unique_ptr<IfStatement> ifStatement();
    std::unique_ptr<WhileStatement> whileStatement();
    std::unique_ptr<ForStatement> forStatement();
    std::unique_ptr<ReturnStatement> returnStatement();
    std::unique_ptr<ExpressionStatement> expressionStatement();
    std::unique_ptr<TryStatement> tryStatement();
    std::unique_ptr<ThrowStatement> throwStatement();
    std::unique_ptr<UsingStatement> usingStatement();
    
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> postfix();
    std::unique_ptr<Expression> primary();
    
    std::string parseType();
    std::vector<std::string> parseModifiers();
    
public:
    Parser(const std::vector<Token>& tokens);
    
    std::unique_ptr<Program> parse();
    
    // Error reporting
    const std::vector<ParseError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors() { errors.clear(); }
};

} // namespace Compiler

#endif // PARSER_H
