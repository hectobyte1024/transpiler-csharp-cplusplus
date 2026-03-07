#include "Parser.h"
#include <stdexcept>

namespace Compiler {

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), current(0), panicMode(false) {}

void Parser::reportError(const std::string& message) {
    if (panicMode) return;  // Don't report cascading errors
    
    Token token = peek();
    errors.emplace_back(message, token.line, token.column);
    panicMode = true;
}

void Parser::synchronize() {
    panicMode = false;
    
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        // Synchronization points: start of declarations
        switch (peek().type) {
            case TokenType::KW_CLASS:
            case TokenType::KW_INTERFACE:
            case TokenType::KW_PUBLIC:
            case TokenType::KW_PRIVATE:
            case TokenType::KW_PROTECTED:
            case TokenType::KW_STATIC:
            case TokenType::KW_IF:
            case TokenType::KW_WHILE:
            case TokenType::KW_FOR:
            case TokenType::KW_RETURN:
            case TokenType::KW_NAMESPACE:
            case TokenType::KW_USING:
                return;
            default:
                break;
        }
        
        advance();
    }
}

bool Parser::hasSynchronizationPoint() {
    TokenType type = peek().type;
    return type == TokenType::SEMICOLON || type == TokenType::RBRACE ||
           type == TokenType::KW_CLASS || type == TokenType::KW_PUBLIC;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::peekNext() {
    if (current + 1 < tokens.size()) {
        return tokens[current + 1];
    }
    return tokens[current];  // Return current if at end
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " at line " + std::to_string(peek().line));
}

std::unique_ptr<Program> Parser::parse() {
    return program();
}

std::unique_ptr<Program> Parser::program() {
    auto prog = std::make_unique<Program>();
    
    while (!isAtEnd()) {
        prog->statements.push_back(declaration());
    }
    
    return prog;
}

std::unique_ptr<Statement> Parser::declaration() {
    try {
        // Using directive
        if (check(TokenType::KW_USING)) {
            return usingDirective();
        }
        
        // Namespace declaration
        if (check(TokenType::KW_NAMESPACE)) {
            return namespaceDeclaration();
        }
        
        // Check for modifiers (public, private, static, etc.)
        std::vector<std::string> modifiers = parseModifiers();
        
        // Class declaration (can have modifiers)
        if (check(TokenType::KW_CLASS)) {
            return classDeclaration();
        }
        
        // Interface declaration (can have modifiers)
        if (check(TokenType::KW_INTERFACE)) {
            return interfaceDeclaration();
        }
        
        if (!modifiers.empty()) {
            // Function or field declaration with modifiers
            std::string type = parseType();
            std::string name = consume(TokenType::IDENTIFIER, "Expected identifier").lexeme;
            
            if (check(TokenType::LPAREN)) {
                // Function declaration
                auto func = std::make_unique<FunctionDeclaration>(type, name);
                func->modifiers = modifiers;
                
                consume(TokenType::LPAREN, "Expected '('");
                
                // Parameters
                if (!check(TokenType::RPAREN)) {
                    do {
                        std::string paramType = parseType();
                        std::string paramName = consume(TokenType::IDENTIFIER, "Expected parameter name").lexeme;
                        func->parameters.push_back({paramType, paramName});
                    } while (match({TokenType::COMMA}));
                }
                
                consume(TokenType::RPAREN, "Expected ')'");
                func->body = blockStatement();
                
                return func;
            } else {
                // Field declaration
                auto var = std::make_unique<VariableDeclaration>(type, name);
                
                if (match({TokenType::OP_ASSIGN})) {
                    var->initializer = expression();
                }
                
                consume(TokenType::SEMICOLON, "Expected ';'");
                return var;
            }
        }
        
        return statement();
    } catch (const std::runtime_error& e) {
        throw;
    }
}

std::unique_ptr<UsingDirective> Parser::usingDirective() {
    consume(TokenType::KW_USING, "Expected 'using'");
    std::string namespaceName = consume(TokenType::IDENTIFIER, "Expected namespace name").lexeme;
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    return std::make_unique<UsingDirective>(namespaceName);
}

std::unique_ptr<NamespaceDeclaration> Parser::namespaceDeclaration() {
    consume(TokenType::KW_NAMESPACE, "Expected 'namespace'");
    std::string name = consume(TokenType::IDENTIFIER, "Expected namespace name").lexeme;
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    auto ns = std::make_unique<NamespaceDeclaration>(name);
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        ns->declarations.push_back(declaration());
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return ns;
}

std::unique_ptr<InterfaceDeclaration> Parser::interfaceDeclaration() {
    consume(TokenType::KW_INTERFACE, "Expected 'interface'");
    std::string name = consume(TokenType::IDENTIFIER, "Expected interface name").lexeme;
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    auto interfaceDecl = std::make_unique<InterfaceDeclaration>(name);
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        std::string returnType = parseType();
        std::string methodName = consume(TokenType::IDENTIFIER, "Expected method name").lexeme;
        
        consume(TokenType::LPAREN, "Expected '('");
        
        auto method = std::make_unique<FunctionDeclaration>(returnType, methodName);
        method->isPureVirtual = true;
        
        if (!check(TokenType::RPAREN)) {
            do {
                std::string paramType = parseType();
                std::string paramName = consume(TokenType::IDENTIFIER, "Expected parameter name").lexeme;
                method->parameters.push_back({paramType, paramName});
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RPAREN, "Expected ')'");
        consume(TokenType::SEMICOLON, "Expected ';'");
        
        interfaceDecl->methods.push_back(std::move(method));
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return interfaceDecl;
}

std::unique_ptr<ClassDeclaration> Parser::classDeclaration() {
    bool isAbstract = false;
    if (check(TokenType::KW_ABSTRACT)) {
        isAbstract = true;
        advance();
    }
    
    consume(TokenType::KW_CLASS, "Expected 'class'");
    std::string name = consume(TokenType::IDENTIFIER, "Expected class name").lexeme;
    
    auto classDecl = std::make_unique<ClassDeclaration>(name);
    classDecl->isAbstract = isAbstract;
    
    // Check for generic parameters
    if (check(TokenType::OP_LESS)) {
        advance(); // consume '<'
        do {
            std::string param = consume(TokenType::IDENTIFIER, "Expected type parameter").lexeme;
            classDecl->templateParams.push_back(param);
        } while (match({TokenType::COMMA}));
        consume(TokenType::OP_GREATER, "Expected '>'");
    }
    
    // Check for inheritance
    if (peek().lexeme == ":") {
        advance(); // consume ':'
        
        // Parse base class and interfaces
        do {
            std::string baseName = consume(TokenType::IDENTIFIER, "Expected base class or interface name").lexeme;
            if (classDecl->baseClass.empty()) {
                classDecl->baseClass = baseName;
            } else {
                classDecl->interfaces.push_back(baseName);
            }
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        std::vector<std::string> modifiers = parseModifiers();
        std::string type = parseType();
        std::string memberName = consume(TokenType::IDENTIFIER, "Expected member name").lexeme;
        
        if (check(TokenType::LPAREN)) {
            // Method
            auto func = std::make_unique<FunctionDeclaration>(type, memberName);
            func->modifiers = modifiers;
            
            consume(TokenType::LPAREN, "Expected '('");
            
            if (!check(TokenType::RPAREN)) {
                do {
                    std::string paramType = parseType();
                    std::string paramName = consume(TokenType::IDENTIFIER, "Expected parameter name").lexeme;
                    func->parameters.push_back({paramType, paramName});
                } while (match({TokenType::COMMA}));
            }
            
            consume(TokenType::RPAREN, "Expected ')'");
            
            // Check for pure virtual
            if (check(TokenType::OP_ASSIGN) && peekNext().lexeme == "0") {
                advance(); // consume '='
                advance(); // consume '0'
                func->isPureVirtual = true;
                consume(TokenType::SEMICOLON, "Expected ';'");
            } else if (check(TokenType::SEMICOLON)) {
                // Abstract method declaration
                advance();
                func->isPureVirtual = true;
            } else {
                func->body = blockStatement();
            }
            
            classDecl->methods.push_back(std::move(func));
        } else {
            // Field
            auto field = std::make_unique<VariableDeclaration>(type, memberName);
            
            if (match({TokenType::OP_ASSIGN})) {
                field->initializer = expression();
            }
            
            consume(TokenType::SEMICOLON, "Expected ';'");
            classDecl->fields.push_back(std::move(field));
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return classDecl;
}

std::unique_ptr<VariableDeclaration> Parser::variableDeclaration() {
    std::string type = parseType();
    std::string name = consume(TokenType::IDENTIFIER, "Expected variable name").lexeme;
    
    std::unique_ptr<Expression> initializer;
    if (match({TokenType::OP_ASSIGN})) {
        initializer = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    return std::make_unique<VariableDeclaration>(type, name, std::move(initializer));
}

std::unique_ptr<Statement> Parser::statement() {
    if (check(TokenType::KW_IF)) return ifStatement();
    if (check(TokenType::KW_WHILE)) return whileStatement();
    if (check(TokenType::KW_FOR)) return forStatement();
    if (check(TokenType::KW_RETURN)) return returnStatement();
    if (check(TokenType::LBRACE)) return blockStatement();
    if (check(TokenType::KW_TRY)) return tryStatement();
    if (check(TokenType::KW_THROW)) return throwStatement();
    if (check(TokenType::KW_USING) && peekNext().type == TokenType::LPAREN) return usingStatement();
    
    // Check if it's a variable declaration (type identifier)
    size_t savedPos = current;
    try {
        // Try to parse as variable declaration
        if (check(TokenType::KW_INT) || check(TokenType::KW_FLOAT) || 
            check(TokenType::KW_DOUBLE) || check(TokenType::KW_STRING) ||
            check(TokenType::KW_BOOL) || check(TokenType::KW_CHAR) ||
            check(TokenType::IDENTIFIER)) {
            
            std::string type = parseType();
            if (check(TokenType::IDENTIFIER)) {
                // It's a variable declaration
                current = savedPos; // Reset and parse properly
                return variableDeclaration();
            }
        }
    } catch (...) {
        // Not a variable declaration, restore position
    }
    current = savedPos;
    
    return expressionStatement();
}

std::unique_ptr<BlockStatement> Parser::blockStatement() {
    consume(TokenType::LBRACE, "Expected '{'");
    
    auto block = std::make_unique<BlockStatement>();
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        block->statements.push_back(statement());
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return block;
}

std::unique_ptr<IfStatement> Parser::ifStatement() {
    consume(TokenType::KW_IF, "Expected 'if'");
    consume(TokenType::LPAREN, "Expected '('");
    auto condition = expression();
    consume(TokenType::RPAREN, "Expected ')'");
    
    auto thenBranch = statement();
    std::unique_ptr<Statement> elseBranch;
    
    if (match({TokenType::KW_ELSE})) {
        elseBranch = statement();
    }
    
    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<WhileStatement> Parser::whileStatement() {
    consume(TokenType::KW_WHILE, "Expected 'while'");
    consume(TokenType::LPAREN, "Expected '('");
    auto condition = expression();
    consume(TokenType::RPAREN, "Expected ')'");
    
    auto body = statement();
    
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<ForStatement> Parser::forStatement() {
    consume(TokenType::KW_FOR, "Expected 'for'");
    consume(TokenType::LPAREN, "Expected '('");
    
    std::unique_ptr<Statement> initializer;
    if (!check(TokenType::SEMICOLON)) {
        initializer = variableDeclaration();
    } else {
        consume(TokenType::SEMICOLON, "Expected ';'");
    }
    
    std::unique_ptr<Expression> condition;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    std::unique_ptr<Expression> increment;
    if (!check(TokenType::RPAREN)) {
        increment = expression();
    }
    consume(TokenType::RPAREN, "Expected ')'");
    
    auto body = statement();
    
    return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), 
                                         std::move(increment), std::move(body));
}

std::unique_ptr<ReturnStatement> Parser::returnStatement() {
    consume(TokenType::KW_RETURN, "Expected 'return'");
    
    std::unique_ptr<Expression> value;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<TryStatement> Parser::tryStatement() {
    consume(TokenType::KW_TRY, "Expected 'try'");
    
    auto tryBlock = blockStatement();
    auto tryStmt = std::make_unique<TryStatement>(std::move(tryBlock));
    
    // Parse catch blocks
    while (check(TokenType::KW_CATCH)) {
        advance(); // consume 'catch'
        consume(TokenType::LPAREN, "Expected '('");
        std::string exceptionType = parseType();
        if (check(TokenType::IDENTIFIER)) {
            advance(); // consume variable name (optional)
        }
        consume(TokenType::RPAREN, "Expected ')'");
        
        auto catchBlock = blockStatement();
        tryStmt->catchBlocks.push_back({exceptionType, std::move(catchBlock)});
    }
    
    // Parse finally block (optional)
    if (check(TokenType::KW_FINALLY)) {
        advance(); // consume 'finally'
        tryStmt->finallyBlock = blockStatement();
    }
    
    return tryStmt;
}

std::unique_ptr<ThrowStatement> Parser::throwStatement() {
    consume(TokenType::KW_THROW, "Expected 'throw'");
    
    std::unique_ptr<Expression> expr;
    if (!check(TokenType::SEMICOLON)) {
        expr = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    return std::make_unique<ThrowStatement>(std::move(expr));
}

std::unique_ptr<UsingStatement> Parser::usingStatement() {
    consume(TokenType::KW_USING, "Expected 'using'");
    consume(TokenType::LPAREN, "Expected '('");
    
    // Parse resource declaration
    std::string type = parseType();
    std::string name = consume(TokenType::IDENTIFIER, "Expected variable name").lexeme;
    
    std::unique_ptr<Expression> initializer;
    if (match({TokenType::OP_ASSIGN})) {
        initializer = expression();
    }
    
    consume(TokenType::RPAREN, "Expected ')'");
    
    auto resource = std::make_unique<VariableDeclaration>(type, name, std::move(initializer));
    auto body = statement();
    
    return std::make_unique<UsingStatement>(std::move(resource), std::move(body));
}

std::unique_ptr<ExpressionStatement> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Expression> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expression> Parser::assignment() {
    auto expr = logicalOr();
    
    if (match({TokenType::OP_ASSIGN, TokenType::OP_PLUS_ASSIGN, TokenType::OP_MINUS_ASSIGN})) {
        Token op = previous();
        auto value = assignment();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.lexeme, std::move(value));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match({TokenType::OP_LOGICAL_OR})) {
        Token op = previous();
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.lexeme, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match({TokenType::OP_LOGICAL_AND})) {
        Token op = previous();
        auto right = equality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.lexeme, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();
    
    while (match({TokenType::OP_EQUAL, TokenType::OP_NOT_EQUAL})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.lexeme, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = term();
    
    while (match({TokenType::OP_LESS, TokenType::OP_LESS_EQUAL, 
                  TokenType::OP_GREATER, TokenType::OP_GREATER_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.lexeme, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    auto expr = factor();
    
    while (match({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.lexeme, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    auto expr = unary();
    
    while (match({TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE, TokenType::OP_MODULO})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.lexeme, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::OP_LOGICAL_NOT, TokenType::OP_MINUS, TokenType::OP_PLUS,
               TokenType::OP_INCREMENT, TokenType::OP_DECREMENT})) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpression>(op.lexeme, std::move(right));
    }
    
    return postfix();
}

std::unique_ptr<Expression> Parser::postfix() {
    auto expr = primary();
    
    while (true) {
        if (match({TokenType::LPAREN})) {
            // Function call
            auto call = std::make_unique<CallExpression>(std::move(expr));
            
            if (!check(TokenType::RPAREN)) {
                do {
                    call->arguments.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            
            consume(TokenType::RPAREN, "Expected ')'");
            expr = std::move(call);
        } else if (match({TokenType::DOT})) {
            // Member access
            std::string member = consume(TokenType::IDENTIFIER, "Expected member name").lexeme;
            expr = std::make_unique<MemberExpression>(std::move(expr), member);
        } else if (match({TokenType::OP_INCREMENT, TokenType::OP_DECREMENT})) {
            // Postfix increment/decrement
            Token op = previous();
            expr = std::make_unique<UnaryExpression>(op.lexeme + "_post", std::move(expr));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    // Literals
    if (check(TokenType::INTEGER_LITERAL)) {
        Token token = advance();
        return std::make_unique<LiteralExpression>(token.lexeme, "int");
    }
    
    if (check(TokenType::FLOAT_LITERAL)) {
        Token token = advance();
        return std::make_unique<LiteralExpression>(token.lexeme, "float");
    }
    
    if (check(TokenType::STRING_LITERAL)) {
        Token token = advance();
        return std::make_unique<LiteralExpression>(token.lexeme, "string");
    }
    
    if (check(TokenType::CHAR_LITERAL)) {
        Token token = advance();
        return std::make_unique<LiteralExpression>(token.lexeme, "char");
    }
    
    if (match({TokenType::KW_TRUE, TokenType::KW_FALSE})) {
        Token token = previous();
        return std::make_unique<LiteralExpression>(token.lexeme, "bool");
    }
    
    if (match({TokenType::KW_NULL})) {
        return std::make_unique<LiteralExpression>("null", "null");
    }
    
    // Identifiers
    if (check(TokenType::IDENTIFIER)) {
        Token token = advance();
        return std::make_unique<IdentifierExpression>(token.lexeme);
    }
    
    // Parenthesized expression
    if (match({TokenType::LPAREN})) {
        auto expr = expression();
        consume(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    
    // New expression
    if (match({TokenType::KW_NEW})) {
        std::string type = parseType();
        consume(TokenType::LPAREN, "Expected '('");
        
        auto call = std::make_unique<CallExpression>(
            std::make_unique<IdentifierExpression>("new_" + type)
        );
        
        if (!check(TokenType::RPAREN)) {
            do {
                call->arguments.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RPAREN, "Expected ')'");
        return call;
    }
    
    throw std::runtime_error("Expected expression at line " + std::to_string(peek().line));
}

std::string Parser::parseType() {
    if (match({TokenType::KW_VOID, TokenType::KW_INT, TokenType::KW_FLOAT, 
               TokenType::KW_DOUBLE, TokenType::KW_STRING, TokenType::KW_BOOL, 
               TokenType::KW_CHAR})) {
        return previous().lexeme;
    }
    
    if (check(TokenType::IDENTIFIER)) {
        return advance().lexeme;
    }
    
    throw std::runtime_error("Expected type at line " + std::to_string(peek().line));
}

std::vector<std::string> Parser::parseModifiers() {
    std::vector<std::string> modifiers;
    
    while (match({TokenType::KW_PUBLIC, TokenType::KW_PRIVATE, 
                  TokenType::KW_PROTECTED, TokenType::KW_STATIC,
                  TokenType::KW_VIRTUAL, TokenType::KW_ABSTRACT,
                  TokenType::KW_OVERRIDE, TokenType::KW_SEALED})) {
        modifiers.push_back(previous().lexeme);
    }
    
    return modifiers;
}

} // namespace Compiler
