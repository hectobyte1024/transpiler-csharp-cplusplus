#include "../src/parser/Parser.h"
#include "../src/lexer/Lexer.h"
#include <iostream>
#include <cassert>

void testVariableDeclaration() {
    std::string code = "int x;";
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    Compiler::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    assert(ast->statements.size() == 1);
    std::cout << "✓ Variable declaration test passed\n";
}

void testFunctionDeclaration() {
    std::string code = R"(
        void MyFunction(int a, int b) {
            return;
        }
    )";
    
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    Compiler::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    std::cout << "✓ Function declaration test passed\n";
}

void testClassDeclaration() {
    std::string code = R"(
        class MyClass {
            public int field;
            public void Method() {
                return;
            }
        }
    )";
    
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    Compiler::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    std::cout << "✓ Class declaration test passed\n";
}

void testIfStatement() {
    std::string code = R"(
        void Test() {
            if (x > 10) {
                y = 1;
            } else {
                y = 0;
            }
        }
    )";
    
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    Compiler::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    std::cout << "✓ If statement test passed\n";
}

void testWhileLoop() {
    std::string code = R"(
        void Test() {
            while (x < 10) {
                x = x + 1;
            }
        }
    )";
    
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    Compiler::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    std::cout << "✓ While loop test passed\n";
}

void testForLoop() {
    std::string code = R"(
        void Test() {
            for (int i = 0; i < 10; i++) {
                sum = sum + i;
            }
        }
    )";
    
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    Compiler::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    std::cout << "✓ For loop test passed\n";
}

void testExpressions() {
    std::string code = "void Test() { x = 1 + 2 * 3; }";
    
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    Compiler::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    std::cout << "✓ Expression test passed\n";
}

int main() {
    std::cout << "Running Parser Tests...\n\n";
    
    testVariableDeclaration();
    testFunctionDeclaration();
    testClassDeclaration();
    testIfStatement();
    testWhileLoop();
    testForLoop();
    testExpressions();
    
    std::cout << "\n✓ All parser tests passed!\n";
    return 0;
}
