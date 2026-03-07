#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

namespace Compiler {

enum class TokenType {
    // Keywords
    KW_CLASS,
    KW_PUBLIC,
    KW_PRIVATE,
    KW_PROTECTED,
    KW_STATIC,
    KW_VOID,
    KW_INT,
    KW_FLOAT,
    KW_DOUBLE,
    KW_STRING,
    KW_BOOL,
    KW_CHAR,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_FOR,
    KW_FOREACH,
    KW_RETURN,
    KW_NEW,
    KW_THIS,
    KW_BASE,
    KW_NAMESPACE,
    KW_USING,
    KW_TRUE,
    KW_FALSE,
    KW_NULL,
    KW_TRY,
    KW_CATCH,
    KW_FINALLY,
    KW_THROW,
    KW_INTERFACE,
    KW_ABSTRACT,
    KW_VIRTUAL,
    KW_OVERRIDE,
    KW_SEALED,
    
    // Identifiers and literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    
    // Operators
    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    OP_ASSIGN,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_LOGICAL_AND,
    OP_LOGICAL_OR,
    OP_LOGICAL_NOT,
    OP_BITWISE_AND,
    OP_BITWISE_OR,
    OP_BITWISE_XOR,
    OP_BITWISE_NOT,
    OP_INCREMENT,
    OP_DECREMENT,
    OP_PLUS_ASSIGN,
    OP_MINUS_ASSIGN,
    
    // Delimiters
    SEMICOLON,
    COMMA,
    DOT,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    
    // Special
    WHITESPACE,
    COMMENT,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
    
    const char* getTypeName() const;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

} // namespace Compiler

#endif // TOKEN_H
