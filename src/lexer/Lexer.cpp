#include "Lexer.h"
#include <stdexcept>
#include <cctype>

namespace Compiler {

TokenPattern::TokenPattern(TokenType type, const std::string& pattern, int priority)
    : type(type), priority(priority) {
    regex = std::make_unique<Regex::RegexEngine>(pattern);
}

Lexer::Lexer(const std::string& source) 
    : source(source), position(0), line(1), column(1) {
    initializeKeywords();
    initializePatterns();
}

void Lexer::initializeKeywords() {
    keywords["class"] = TokenType::KW_CLASS;
    keywords["public"] = TokenType::KW_PUBLIC;
    keywords["private"] = TokenType::KW_PRIVATE;
    keywords["protected"] = TokenType::KW_PROTECTED;
    keywords["static"] = TokenType::KW_STATIC;
    keywords["void"] = TokenType::KW_VOID;
    keywords["int"] = TokenType::KW_INT;
    keywords["float"] = TokenType::KW_FLOAT;
    keywords["double"] = TokenType::KW_DOUBLE;
    keywords["string"] = TokenType::KW_STRING;
    keywords["bool"] = TokenType::KW_BOOL;
    keywords["char"] = TokenType::KW_CHAR;
    keywords["if"] = TokenType::KW_IF;
    keywords["else"] = TokenType::KW_ELSE;
    keywords["while"] = TokenType::KW_WHILE;
    keywords["for"] = TokenType::KW_FOR;
    keywords["foreach"] = TokenType::KW_FOREACH;
    keywords["return"] = TokenType::KW_RETURN;
    keywords["new"] = TokenType::KW_NEW;
    keywords["this"] = TokenType::KW_THIS;
    keywords["base"] = TokenType::KW_BASE;
    keywords["namespace"] = TokenType::KW_NAMESPACE;
    keywords["using"] = TokenType::KW_USING;
    keywords["true"] = TokenType::KW_TRUE;
    keywords["false"] = TokenType::KW_FALSE;
    keywords["null"] = TokenType::KW_NULL;
    keywords["try"] = TokenType::KW_TRY;
    keywords["catch"] = TokenType::KW_CATCH;
    keywords["finally"] = TokenType::KW_FINALLY;
    keywords["throw"] = TokenType::KW_THROW;
    keywords["interface"] = TokenType::KW_INTERFACE;
    keywords["abstract"] = TokenType::KW_ABSTRACT;
    keywords["virtual"] = TokenType::KW_VIRTUAL;
    keywords["override"] = TokenType::KW_OVERRIDE;
    keywords["sealed"] = TokenType::KW_SEALED;
}

void Lexer::initializePatterns() {
    // Note: These patterns use our custom regex engine
    // Pattern priorities ensure correct token recognition
    
    // Identifiers: [a-zA-Z_][a-zA-Z0-9_]*
    // Note: We handle this manually for better performance
    
    // Integer literals: [0-9]+
    // Float literals: [0-9]+\.[0-9]+
    // Note: We handle these manually
    
    // We'll use manual scanning for most tokens for efficiency
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[position];
}

char Lexer::peekNext() {
    if (position + 1 >= source.length()) return '\0';
    return source[position + 1];
}

char Lexer::advance() {
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Lexer::isAtEnd() {
    return position >= source.length();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && peekNext() == '/') {
        // Single-line comment
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
    } else if (peek() == '/' && peekNext() == '*') {
        // Multi-line comment
        advance(); // consume '/'
        advance(); // consume '*'
        
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '/') {
                advance(); // consume '*'
                advance(); // consume '/'
                break;
            }
            advance();
        }
    }
}

Token Lexer::scanIdentifierOrKeyword() {
    int startLine = line;
    int startColumn = column;
    size_t start = position;
    
    // First character must be letter or underscore
    if (!std::isalpha(peek()) && peek() != '_') {
        return Token(TokenType::UNKNOWN, "", startLine, startColumn);
    }
    
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }
    
    std::string lexeme = source.substr(start, position - start);
    
    // Check if it's a keyword
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return Token(it->second, lexeme, startLine, startColumn);
    }
    
    return Token(TokenType::IDENTIFIER, lexeme, startLine, startColumn);
}

Token Lexer::scanNumber() {
    int startLine = line;
    int startColumn = column;
    size_t start = position;
    
    bool isFloat = false;
    
    // Scan integer part
    while (!isAtEnd() && std::isdigit(peek())) {
        advance();
    }
    
    // Check for decimal point
    if (peek() == '.' && std::isdigit(peekNext())) {
        isFloat = true;
        advance(); // consume '.'
        
        // Scan fractional part
        while (!isAtEnd() && std::isdigit(peek())) {
            advance();
        }
    }
    
    std::string lexeme = source.substr(start, position - start);
    TokenType type = isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL;
    
    return Token(type, lexeme, startLine, startColumn);
}

Token Lexer::scanString() {
    int startLine = line;
    int startColumn = column;
    size_t start = position;
    
    advance(); // consume opening '"'
    
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            advance(); // consume '\\'
            if (!isAtEnd()) {
                advance(); // consume escaped character
            }
        } else {
            advance();
        }
    }
    
    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string literal");
    }
    
    advance(); // consume closing '"'
    
    std::string lexeme = source.substr(start, position - start);
    return Token(TokenType::STRING_LITERAL, lexeme, startLine, startColumn);
}

Token Lexer::scanChar() {
    int startLine = line;
    int startColumn = column;
    size_t start = position;
    
    advance(); // consume opening '\''
    
    if (peek() == '\\') {
        advance(); // consume '\\'
        if (!isAtEnd()) {
            advance(); // consume escaped character
        }
    } else if (!isAtEnd()) {
        advance(); // consume character
    }
    
    if (peek() != '\'') {
        throw std::runtime_error("Unterminated character literal");
    }
    
    advance(); // consume closing '\''
    
    std::string lexeme = source.substr(start, position - start);
    return Token(TokenType::CHAR_LITERAL, lexeme, startLine, startColumn);
}

Token Lexer::scanToken() {
    skipWhitespace();
    
    // Skip comments
    while (peek() == '/' && (peekNext() == '/' || peekNext() == '*')) {
        skipComment();
        skipWhitespace();
    }
    
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }
    
    int startLine = line;
    int startColumn = column;
    char c = peek();
    
    // Identifiers and keywords
    if (std::isalpha(c) || c == '_') {
        return scanIdentifierOrKeyword();
    }
    
    // Numbers
    if (std::isdigit(c)) {
        return scanNumber();
    }
    
    // String literals
    if (c == '"') {
        return scanString();
    }
    
    // Character literals
    if (c == '\'') {
        return scanChar();
    }
    
    // Operators and delimiters
    advance(); // consume first character
    
    switch (c) {
        // Two-character operators
        case '+':
            if (peek() == '+') {
                advance();
                return Token(TokenType::OP_INCREMENT, "++", startLine, startColumn);
            } else if (peek() == '=') {
                advance();
                return Token(TokenType::OP_PLUS_ASSIGN, "+=", startLine, startColumn);
            }
            return Token(TokenType::OP_PLUS, "+", startLine, startColumn);
            
        case '-':
            if (peek() == '-') {
                advance();
                return Token(TokenType::OP_DECREMENT, "--", startLine, startColumn);
            } else if (peek() == '=') {
                advance();
                return Token(TokenType::OP_MINUS_ASSIGN, "-=", startLine, startColumn);
            }
            return Token(TokenType::OP_MINUS, "-", startLine, startColumn);
            
        case '=':
            if (peek() == '=') {
                advance();
                return Token(TokenType::OP_EQUAL, "==", startLine, startColumn);
            }
            return Token(TokenType::OP_ASSIGN, "=", startLine, startColumn);
            
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TokenType::OP_NOT_EQUAL, "!=", startLine, startColumn);
            }
            return Token(TokenType::OP_LOGICAL_NOT, "!", startLine, startColumn);
            
        case '<':
            if (peek() == '=') {
                advance();
                return Token(TokenType::OP_LESS_EQUAL, "<=", startLine, startColumn);
            }
            return Token(TokenType::OP_LESS, "<", startLine, startColumn);
            
        case '>':
            if (peek() == '=') {
                advance();
                return Token(TokenType::OP_GREATER_EQUAL, ">=", startLine, startColumn);
            }
            return Token(TokenType::OP_GREATER, ">", startLine, startColumn);
            
        case '&':
            if (peek() == '&') {
                advance();
                return Token(TokenType::OP_LOGICAL_AND, "&&", startLine, startColumn);
            }
            return Token(TokenType::OP_BITWISE_AND, "&", startLine, startColumn);
            
        case '|':
            if (peek() == '|') {
                advance();
                return Token(TokenType::OP_LOGICAL_OR, "||", startLine, startColumn);
            }
            return Token(TokenType::OP_BITWISE_OR, "|", startLine, startColumn);
            
        // Single-character operators
        case '*': return Token(TokenType::OP_MULTIPLY, "*", startLine, startColumn);
        case '/': return Token(TokenType::OP_DIVIDE, "/", startLine, startColumn);
        case '%': return Token(TokenType::OP_MODULO, "%", startLine, startColumn);
        case '^': return Token(TokenType::OP_BITWISE_XOR, "^", startLine, startColumn);
        case '~': return Token(TokenType::OP_BITWISE_NOT, "~", startLine, startColumn);
        
        // Delimiters
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
        case ',': return Token(TokenType::COMMA, ",", startLine, startColumn);
        case '.': return Token(TokenType::DOT, ".", startLine, startColumn);
        case '(': return Token(TokenType::LPAREN, "(", startLine, startColumn);
        case ')': return Token(TokenType::RPAREN, ")", startLine, startColumn);
        case '{': return Token(TokenType::LBRACE, "{", startLine, startColumn);
        case '}': return Token(TokenType::RBRACE, "}", startLine, startColumn);
        case '[': return Token(TokenType::LBRACKET, "[", startLine, startColumn);
        case ']': return Token(TokenType::RBRACKET, "]", startLine, startColumn);
        
        default:
            return Token(TokenType::UNKNOWN, std::string(1, c), startLine, startColumn);
    }
}

Token Lexer::nextToken() {
    return scanToken();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = scanToken();
        tokens.push_back(token);
        
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }
    
    return tokens;
}

void Lexer::reset() {
    position = 0;
    line = 1;
    column = 1;
}

} // namespace Compiler
