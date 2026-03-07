#include "Token.h"

namespace Compiler {

const char* Token::getTypeName() const {
    switch (type) {
        case TokenType::KW_CLASS: return "KW_CLASS";
        case TokenType::KW_PUBLIC: return "KW_PUBLIC";
        case TokenType::KW_PRIVATE: return "KW_PRIVATE";
        case TokenType::KW_PROTECTED: return "KW_PROTECTED";
        case TokenType::KW_STATIC: return "KW_STATIC";
        case TokenType::KW_VOID: return "KW_VOID";
        case TokenType::KW_INT: return "KW_INT";
        case TokenType::KW_FLOAT: return "KW_FLOAT";
        case TokenType::KW_DOUBLE: return "KW_DOUBLE";
        case TokenType::KW_STRING: return "KW_STRING";
        case TokenType::KW_BOOL: return "KW_BOOL";
        case TokenType::KW_CHAR: return "KW_CHAR";
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_ELSE: return "KW_ELSE";
        case TokenType::KW_WHILE: return "KW_WHILE";
        case TokenType::KW_FOR: return "KW_FOR";
        case TokenType::KW_FOREACH: return "KW_FOREACH";
        case TokenType::KW_RETURN: return "KW_RETURN";
        case TokenType::KW_NEW: return "KW_NEW";
        case TokenType::KW_THIS: return "KW_THIS";
        case TokenType::KW_BASE: return "KW_BASE";
        case TokenType::KW_NAMESPACE: return "KW_NAMESPACE";
        case TokenType::KW_USING: return "KW_USING";
        case TokenType::KW_TRUE: return "KW_TRUE";
        case TokenType::KW_FALSE: return "KW_FALSE";
        case TokenType::KW_NULL: return "KW_NULL";
        case TokenType::KW_TRY: return "KW_TRY";
        case TokenType::KW_CATCH: return "KW_CATCH";
        case TokenType::KW_FINALLY: return "KW_FINALLY";
        case TokenType::KW_THROW: return "KW_THROW";
        case TokenType::KW_INTERFACE: return "KW_INTERFACE";
        case TokenType::KW_ABSTRACT: return "KW_ABSTRACT";
        case TokenType::KW_VIRTUAL: return "KW_VIRTUAL";
        case TokenType::KW_OVERRIDE: return "KW_OVERRIDE";
        case TokenType::KW_SEALED: return "KW_SEALED";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
        case TokenType::OP_PLUS: return "OP_PLUS";
        case TokenType::OP_MINUS: return "OP_MINUS";
        case TokenType::OP_MULTIPLY: return "OP_MULTIPLY";
        case TokenType::OP_DIVIDE: return "OP_DIVIDE";
        case TokenType::OP_MODULO: return "OP_MODULO";
        case TokenType::OP_ASSIGN: return "OP_ASSIGN";
        case TokenType::OP_EQUAL: return "OP_EQUAL";
        case TokenType::OP_NOT_EQUAL: return "OP_NOT_EQUAL";
        case TokenType::OP_LESS: return "OP_LESS";
        case TokenType::OP_LESS_EQUAL: return "OP_LESS_EQUAL";
        case TokenType::OP_GREATER: return "OP_GREATER";
        case TokenType::OP_GREATER_EQUAL: return "OP_GREATER_EQUAL";
        case TokenType::OP_LOGICAL_AND: return "OP_LOGICAL_AND";
        case TokenType::OP_LOGICAL_OR: return "OP_LOGICAL_OR";
        case TokenType::OP_LOGICAL_NOT: return "OP_LOGICAL_NOT";
        case TokenType::OP_BITWISE_AND: return "OP_BITWISE_AND";
        case TokenType::OP_BITWISE_OR: return "OP_BITWISE_OR";
        case TokenType::OP_BITWISE_XOR: return "OP_BITWISE_XOR";
        case TokenType::OP_BITWISE_NOT: return "OP_BITWISE_NOT";
        case TokenType::OP_INCREMENT: return "OP_INCREMENT";
        case TokenType::OP_DECREMENT: return "OP_DECREMENT";
        case TokenType::OP_PLUS_ASSIGN: return "OP_PLUS_ASSIGN";
        case TokenType::OP_MINUS_ASSIGN: return "OP_MINUS_ASSIGN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << token.getTypeName() << ", '" << token.lexeme 
       << "', " << token.line << ":" << token.column << ")";
    return os;
}

} // namespace Compiler
