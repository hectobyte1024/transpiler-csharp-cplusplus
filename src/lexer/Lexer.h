#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include "../regex/RegexParser.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Compiler {

// Token pattern with priority
struct TokenPattern {
    TokenType type;
    std::unique_ptr<Regex::RegexEngine> regex;
    int priority; // Higher priority checked first
    
    TokenPattern(TokenType type, const std::string& pattern, int priority);
};

// Lexical Analyzer based on Finite Automata
class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    std::vector<TokenPattern> patterns;
    std::map<std::string, TokenType> keywords;
    
    void initializePatterns();
    void initializeKeywords();
    
    char peek();
    char peekNext();
    char advance();
    bool isAtEnd();
    
    Token scanToken();
    Token matchPattern();
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();
    Token scanChar();
    Token scanOperator();
    void skipWhitespace();
    void skipComment();
    
public:
    Lexer(const std::string& source);
    
    std::vector<Token> tokenize();
    Token nextToken();
    
    void reset();
    int getLine() const { return line; }
    int getColumn() const { return column; }
};

} // namespace Compiler

#endif // LEXER_H
