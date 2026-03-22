#include "Lexer.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>

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
    // Token patterns using custom regex engine
    // Higher priority patterns are checked first for maximal munch
    // Patterns are processed with longest-match-first strategy
    // Note: All regex metacharacters are properly escaped
    
    // Numeric literals (before identifiers to catch digits first)
    patterns.push_back(TokenPattern(TokenType::FLOAT_LITERAL, "[0-9]+\\.[0-9]+", 150));
    patterns.push_back(TokenPattern(TokenType::INTEGER_LITERAL, "[0-9]+", 140));
    
    // Identifiers (keywords handled separately after pattern match)
    patterns.push_back(TokenPattern(TokenType::IDENTIFIER, "[a-zA-Z_][a-zA-Z0-9_]*", 130));
    
    // Two-character operators (before single-char to prefer longer match)
    patterns.push_back(TokenPattern(TokenType::OP_INCREMENT, "\\+\\+", 120));
    patterns.push_back(TokenPattern(TokenType::OP_DECREMENT, "--", 120));
    patterns.push_back(TokenPattern(TokenType::OP_PLUS_ASSIGN, "\\+=", 120));
    patterns.push_back(TokenPattern(TokenType::OP_MINUS_ASSIGN, "-=", 120));
    patterns.push_back(TokenPattern(TokenType::OP_EQUAL, "==", 120));
    patterns.push_back(TokenPattern(TokenType::OP_NOT_EQUAL, "!=", 120));
    patterns.push_back(TokenPattern(TokenType::OP_LESS_EQUAL, "<=", 120));
    patterns.push_back(TokenPattern(TokenType::OP_GREATER_EQUAL, ">=", 120));
    patterns.push_back(TokenPattern(TokenType::OP_LOGICAL_AND, "&&", 120));
    patterns.push_back(TokenPattern(TokenType::OP_LOGICAL_OR, "\\|\\|", 120));  // Escaped
    
    // Single-character operators and delimiters (lowest priority)
    patterns.push_back(TokenPattern(TokenType::OP_PLUS, "\\+", 100));
    patterns.push_back(TokenPattern(TokenType::OP_MINUS, "-", 100));
    patterns.push_back(TokenPattern(TokenType::OP_MULTIPLY, "\\*", 100));
    patterns.push_back(TokenPattern(TokenType::OP_DIVIDE, "/", 100));
    patterns.push_back(TokenPattern(TokenType::OP_MODULO, "%", 100));
    patterns.push_back(TokenPattern(TokenType::OP_ASSIGN, "=", 100));
    patterns.push_back(TokenPattern(TokenType::OP_LOGICAL_NOT, "!", 100));
    patterns.push_back(TokenPattern(TokenType::OP_LESS, "<", 100));
    patterns.push_back(TokenPattern(TokenType::OP_GREATER, ">", 100));
    patterns.push_back(TokenPattern(TokenType::OP_BITWISE_AND, "&", 100));
    patterns.push_back(TokenPattern(TokenType::OP_BITWISE_OR, "\\|", 100));   // Escaped
    patterns.push_back(TokenPattern(TokenType::OP_BITWISE_XOR, "\\^", 100));  // Escaped
    patterns.push_back(TokenPattern(TokenType::OP_BITWISE_NOT, "~", 100));
    
    // Delimiters (same priority)
    patterns.push_back(TokenPattern(TokenType::SEMICOLON, ";", 100));
    patterns.push_back(TokenPattern(TokenType::COMMA, ",", 100));
    patterns.push_back(TokenPattern(TokenType::DOT, "\\.", 100));             // Escaped (. is wildcard in regex)
    patterns.push_back(TokenPattern(TokenType::LPAREN, "\\(", 100));          // Escaped
    patterns.push_back(TokenPattern(TokenType::RPAREN, "\\)", 100));          // Escaped
    patterns.push_back(TokenPattern(TokenType::LBRACE, "\\{", 100));          // Escaped
    patterns.push_back(TokenPattern(TokenType::RBRACE, "\\}", 100));          // Escaped
    patterns.push_back(TokenPattern(TokenType::LBRACKET, "\\[", 100));        // Escaped
    patterns.push_back(TokenPattern(TokenType::RBRACKET, "\\]", 100));        // Escaped
    
    // Sort by priority (highest first)
    std::sort(patterns.begin(), patterns.end(),
              [](const TokenPattern& a, const TokenPattern& b) {
                  return a.priority > b.priority;
              });
}

// Attempt to match patterns starting at current position
// Returns Token with longest match, or UNKNOWN if no match
// Uses maximal-munch principle: longest match wins
Token Lexer::matchPattern() {
    int startLine = line;
    int startColumn = column;
    
    Token bestMatch(TokenType::UNKNOWN, "", startLine, startColumn);
    size_t longestLength = 0;
    
    // Try each pattern and find longest match
    for (const auto& pattern : patterns) {
        // Try progressively longer substrings from current position
        // to find longest match (maximal munch)
        for (size_t len = 1; len <= source.length() - position; len++) {
            std::string candidate = source.substr(position, len);
            
            // Check if this substring matches the pattern
            if (pattern.regex->match(candidate)) {
                // Found a match - track if it's the longest so far
                if (len > longestLength) {
                    longestLength = len;
                    bestMatch = Token(pattern.type, candidate, startLine, startColumn);
                }
            } else {
                // If match fails, longer strings won't match either
                break;
            }
        }
    }
    
    // If we found a match, advance our position
    if (longestLength > 0) {
        for (size_t i = 0; i < longestLength; i++) {
            advance();
        }
    }
    
    return bestMatch;
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
    
    // String literals (manual scanning for escape sequences)
    if (c == '"') {
        return scanString();
    }
    
    // Character literals (manual scanning for escape sequences)
    if (c == '\'') {
        return scanChar();
    }
    
    // Try pattern matching via regex engine
    // This handles identifiers, keywords, numbers, and operators
    Token patternMatch = matchPattern();
    if (patternMatch.type != TokenType::UNKNOWN) {
        // Check if identifier matched is actually a keyword
        if (patternMatch.type == TokenType::IDENTIFIER) {
            auto it = keywords.find(patternMatch.lexeme);
            if (it != keywords.end()) {
                return Token(it->second, patternMatch.lexeme, startLine, startColumn);
            }
        }
        return patternMatch;
    }
    
    // If no pattern matched, consume one character as unknown
    advance();
    return Token(TokenType::UNKNOWN, std::string(1, c), startLine, startColumn);
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
