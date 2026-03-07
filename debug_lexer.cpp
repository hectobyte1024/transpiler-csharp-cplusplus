#include "lexer/Lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.cs>\n";
        return 1;
    }
    
    std::string sourceCode = readFile(argv[1]);
    Compiler::Lexer lexer(sourceCode);
    std::vector<Compiler::Token> tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        std::cout << token << "\n";
    }
    
    return 0;
}
