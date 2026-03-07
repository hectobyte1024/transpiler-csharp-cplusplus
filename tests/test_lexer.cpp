#include "../src/lexer/Lexer.h"
#include <iostream>
#include <cassert>

void testKeywords() {
    std::string code = "class public private static void int";
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() == 7); // 6 keywords + EOF
    assert(tokens[0].type == Compiler::TokenType::KW_CLASS);
    assert(tokens[1].type == Compiler::TokenType::KW_PUBLIC);
    assert(tokens[2].type == Compiler::TokenType::KW_PRIVATE);
    assert(tokens[3].type == Compiler::TokenType::KW_STATIC);
    assert(tokens[4].type == Compiler::TokenType::KW_VOID);
    assert(tokens[5].type == Compiler::TokenType::KW_INT);
    
    std::cout << "✓ Keywords test passed\n";
}

void testIdentifiers() {
    std::string code = "myVariable _private x1 Value123";
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() == 5); // 4 identifiers + EOF
    assert(tokens[0].type == Compiler::TokenType::IDENTIFIER);
    assert(tokens[0].lexeme == "myVariable");
    assert(tokens[1].type == Compiler::TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "_private");
    
    std::cout << "✓ Identifiers test passed\n";
}

void testNumbers() {
    std::string code = "42 3.14 0 100.5";
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() == 5); // 4 numbers + EOF
    assert(tokens[0].type == Compiler::TokenType::INTEGER_LITERAL);
    assert(tokens[0].lexeme == "42");
    assert(tokens[1].type == Compiler::TokenType::FLOAT_LITERAL);
    assert(tokens[1].lexeme == "3.14");
    
    std::cout << "✓ Numbers test passed\n";
}

void testOperators() {
    std::string code = "+ - * / == != <= >= ++ --";
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() == 11); // 10 operators + EOF
    assert(tokens[0].type == Compiler::TokenType::OP_PLUS);
    assert(tokens[1].type == Compiler::TokenType::OP_MINUS);
    assert(tokens[2].type == Compiler::TokenType::OP_MULTIPLY);
    assert(tokens[3].type == Compiler::TokenType::OP_DIVIDE);
    assert(tokens[4].type == Compiler::TokenType::OP_EQUAL);
    assert(tokens[5].type == Compiler::TokenType::OP_NOT_EQUAL);
    
    std::cout << "✓ Operators test passed\n";
}

void testStringLiteral() {
    std::string code = "\"Hello, World!\" \"test\"";
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() == 3); // 2 strings + EOF
    assert(tokens[0].type == Compiler::TokenType::STRING_LITERAL);
    assert(tokens[0].lexeme == "\"Hello, World!\"");
    
    std::cout << "✓ String literals test passed\n";
}

void testComments() {
    std::string code = "int x; // This is a comment\nint y;";
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    // Comments should be skipped
    bool foundX = false, foundY = false;
    for (const auto& token : tokens) {
        if (token.type == Compiler::TokenType::IDENTIFIER) {
            if (token.lexeme == "x") foundX = true;
            if (token.lexeme == "y") foundY = true;
        }
    }
    
    assert(foundX && foundY);
    std::cout << "✓ Comments test passed\n";
}

void testCompleteProgram() {
    std::string code = R"(
        class MyClass {
            public int x;
            public void Method() {
                x = 10 + 20;
            }
        }
    )";
    
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() > 10); // Should have many tokens
    std::cout << "✓ Complete program test passed\n";
}

int main() {
    std::cout << "Running Lexer Tests...\n\n";
    
    testKeywords();
    testIdentifiers();
    testNumbers();
    testOperators();
    testStringLiteral();
    testComments();
    testCompleteProgram();
    
    std::cout << "\n✓ All lexer tests passed!\n";
    return 0;
}
