#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "codegen/CodeGenerator.h"
#include "ir/IRGenerator.h"
#include "ir/IROptimizer.h"
#include <iostream>
#include <fstream>
#include <sstream>

void printUsage(const char* programName) {
    std::cout << "C# to C++ Source-to-Source Compiler\n";
    std::cout << "Usage: " << programName << " <input.cs> [output.cpp] [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --emit-ir       Emit intermediate representation instead of C++\n";
    std::cout << "  --optimize      Run optimization passes on IR\n";
    std::cout << "  --opt-level=N   Optimization level (1-3, default: 2)\n";
    std::cout << "  --verbose       Show detailed compilation information\n";
    std::cout << "\nFeatures:\n";
    std::cout << "  - Custom Regular Expression Engine (Thompson Construction)\n";
    std::cout << "  - Subset Construction (NFA to DFA conversion)\n";
    std::cout << "  - DFA Simulation for token recognition\n";
    std::cout << "  - Manually constructed lexical analyzer\n";
    std::cout << "  - Recursive descent parser with error recovery\n";
    std::cout << "  - Intermediate representation (IR) layer\n";
    std::cout << "  - IR optimization passes\n";
    std::cout << "  - C++ code generation with pretty printing\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not write to file: " + filename);
    }
    
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputFile;
    std::string outputFile = "output.cpp";
    bool emitIR = false;
    bool optimize = false;
    int optLevel = 2;
    bool verbose = false;
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--emit-ir") {
            emitIR = true;
        } else if (arg == "--optimize") {
            optimize = true;
        } else if (arg.find("--opt-level=") == 0) {
            optLevel = std::stoi(arg.substr(12));
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                outputFile = arg;
            }
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // Adjust output file extension for IR
    if (emitIR && outputFile == "output.cpp") {
        outputFile = "output.ir";
    }
    
    // Determine header file name
    std::string headerFile = outputFile;
    size_t dotPos = headerFile.find_last_of('.');
    if (dotPos != std::string::npos) {
        headerFile = headerFile.substr(0, dotPos) + ".h";
    } else {
        headerFile += ".h";
    }
    
    try {
        std::cout << "Reading input file: " << inputFile << "\n";
        std::string sourceCode = readFile(inputFile);
        
        // Lexical Analysis
        std::cout << "Performing lexical analysis...\n";
        Compiler::Lexer lexer(sourceCode);
        std::vector<Compiler::Token> tokens = lexer.tokenize();
        
        std::cout << "Found " << tokens.size() << " tokens\n";
        
        // Print tokens (optional, for debugging)
        if (verbose) {
            std::cout << "\nTokens:\n";
            for (const auto& token : tokens) {
                std::cout << "  " << token << "\n";
            }
        }
        
        // Parsing
        std::cout << "\nPerforming syntactic analysis...\n";
        Compiler::Parser parser(tokens);
        std::unique_ptr<Compiler::Program> ast = parser.parse();
        
        // Check for parse errors
        if (parser.hasErrors()) {
            std::cerr << "\nParse errors encountered:\n";
            for (const auto& error : parser.getErrors()) {
                std::cerr << "  " << error.toString() << "\n";
            }
            return 1;
        }
        
        std::cout << "AST constructed successfully\n";
        
        // IR Generation
        if (emitIR || optimize) {
            std::cout << "\nGenerating intermediate representation...\n";
            Compiler::IRGenerator irGen;
            std::unique_ptr<Compiler::IR::Module> irModule = irGen.generate(*ast);
            
            if (verbose) {
                std::cout << "\nUnoptimized IR:\n";
                std::cout << irModule->toString() << "\n";
            }
            
            // Optimization
            if (optimize) {
                std::cout << "Running IR optimization passes (level " << optLevel << ")...\n";
                Compiler::IR::IROptimizer optimizer(irModule.get());
                optimizer.optimize(optLevel * 5);
                
                if (verbose) {
                    std::cout << "\nOptimized IR:\n";
                    std::cout << irModule->toString() << "\n";
                }
                
                std::cout << "Optimization completed\n";
            }
            
            // Emit IR if requested
            if (emitIR) {
                std::string irCode = irModule->toString();
                std::cout << "Writing IR to: " << outputFile << "\n";
                writeFile(outputFile, irCode);
                
                std::cout << "\nIR generation successful!\n";
                std::cout << "Generated IR: " << outputFile << "\n";
                return 0;
            }
            
            // Continue to C++ generation after optimization
            // (In future: CodeGenerator could consume optimized IR)
        }
        
        // Code Generation
        std::cout << "\nGenerating C++ code...\n";
        Compiler::CodeGenerator codegen;
        
        // Generate header file
        std::string headerCode = codegen.generateHeader(*ast);
        std::cout << "Writing header to: " << headerFile << "\n";
        writeFile(headerFile, headerCode);
        
        // Generate source file
        std::string sourceFileCode = codegen.generateSource(*ast);
        std::cout << "Writing source to: " << outputFile << "\n";
        writeFile(outputFile, sourceFileCode);
        
        std::cout << "\nCompilation successful!\n";
        std::cout << "Generated C++ header: " << headerFile << "\n";
        std::cout << "Generated C++ source: " << outputFile << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
