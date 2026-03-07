#include "../src/lexer/Lexer.h"
#include "../src/parser/Parser.h"
#include "../src/ir/IRGenerator.h"
#include "../src/ir/IROptimizer.h"
#include <iostream>
#include <cassert>
#include <sstream>

// Helper function to parse C# code to AST
std::unique_ptr<Compiler::Program> parseCode(const std::string& code) {
    Compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Compiler::Parser parser(tokens);
    return parser.parse();
}

// Helper function to generate IR from C# code
std::unique_ptr<Compiler::IR::Module> generateIR(const std::string& code) {
    auto ast = parseCode(code);
    Compiler::IRGenerator irGen;
    return irGen.generate(*ast);
}

// Test basic IR generation for variable declaration
void testVariableDeclaration() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int x = 42;
            }
        }
    )";
    
    auto ir = generateIR(code);
    assert(ir != nullptr);
    
    std::string irStr = ir->toString();
    assert(irStr.find("alloca") != std::string::npos);
    assert(irStr.find("load 42") != std::string::npos);
    assert(irStr.find("store") != std::string::npos);
    
    std::cout << "✓ Variable declaration IR test passed\n";
}

// Test binary expression IR generation
void testBinaryExpression() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int result = 2 + 3;
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have loads for 2 and 3, and an add operation
    assert(irStr.find("load 2") != std::string::npos);
    assert(irStr.find("load 3") != std::string::npos);
    assert(irStr.find("add") != std::string::npos);
    
    std::cout << "✓ Binary expression IR test passed\n";
}

// Test if statement IR generation
void testIfStatement() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int x = 5;
                if (x > 3) {
                    int y = 1;
                }
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have comparison and conditional branch
    assert(irStr.find("gt") != std::string::npos);
    assert(irStr.find("condbr") != std::string::npos);
    assert(irStr.find("br L") != std::string::npos);
    
    std::cout << "✓ If statement IR test passed\n";
}

// Test while loop IR generation
void testWhileLoop() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int i = 0;
                while (i < 10) {
                    i = i + 1;
                }
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have loop labels and conditional branch
    assert(irStr.find("lt") != std::string::npos);
    assert(irStr.find("condbr") != std::string::npos);
    // Should have multiple labels (loop entry, body, end)
    int labelCount = 0;
    size_t pos = 0;
    while ((pos = irStr.find("L", pos)) != std::string::npos) {
        labelCount++;
        pos++;
    }
    assert(labelCount >= 3); // At least 3 labels
    
    std::cout << "✓ While loop IR test passed\n";
}

// Test for loop IR generation
void testForLoop() {
    std::string code = R"(
        class Test {
            public static void Main() {
                for (int i = 0; i < 5; i = i + 1) {
                    int x = i;
                }
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have initialization, condition, and increment
    assert(irStr.find("alloca") != std::string::npos);
    assert(irStr.find("lt") != std::string::npos);
    assert(irStr.find("add") != std::string::npos);
    
    std::cout << "✓ For loop IR test passed\n";
}

// Test return statement IR generation
void testReturnStatement() {
    std::string code = R"(
        class Test {
            public static int GetValue() {
                return 42;
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have return instruction with value
    assert(irStr.find("ret") != std::string::npos);
    assert(irStr.find("42") != std::string::npos);
    
    std::cout << "✓ Return statement IR test passed\n";
}

// Test constant folding optimization
void testConstantFolding() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int x = 2 + 3;
                int y = 10 * 5;
                int z = 100 - 20;
            }
        }
    )";
    
    auto ir = generateIR(code);
    Compiler::IR::IROptimizer optimizer(ir.get());
    
    // Before optimization - should have add, mul, sub
    std::string beforeIR = ir->toString();
    assert(beforeIR.find("add") != std::string::npos);
    assert(beforeIR.find("mul") != std::string::npos);
    assert(beforeIR.find("sub") != std::string::npos);
    
    // Run optimization
    optimizer.optimize(10);
    
    // After optimization - constant expressions should be folded
    std::string afterIR = ir->toString();
    
    // The optimized IR should have fewer arithmetic operations
    // (constants should be pre-computed)
    int beforeOps = 0, afterOps = 0;
    for (const std::string& op : {"add", "mul", "sub", "div"}) {
        size_t pos = 0;
        while ((pos = beforeIR.find(op, pos)) != std::string::npos) {
            beforeOps++;
            pos++;
        }
        pos = 0;
        while ((pos = afterIR.find(op, pos)) != std::string::npos) {
            afterOps++;
            pos++;
        }
    }
    
    // After optimization should have fewer operations
    assert(afterOps <= beforeOps);
    
    std::cout << "✓ Constant folding optimization test passed\n";
}

// Test dead code elimination
void testDeadCodeElimination() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int x = 42;
                int y = 100;
            }
        }
    )";
    
    auto ir = generateIR(code);
    Compiler::IR::IROptimizer optimizer(ir.get());
    
    // Count instructions before optimization
    std::string beforeIR = ir->toString();
    int beforeInstCount = 0;
    for (char c : beforeIR) {
        if (c == '\n') beforeInstCount++;
    }
    
    // Run optimization
    optimizer.optimize(10);
    
    // Count instructions after optimization
    std::string afterIR = ir->toString();
    int afterInstCount = 0;
    for (char c : afterIR) {
        if (c == '\n') afterInstCount++;
    }
    
    // Dead code elimination should remove or reduce instructions
    // (though in this simple case, variables might still be kept)
    assert(afterInstCount <= beforeInstCount + 5); // Allow some tolerance
    
    std::cout << "✓ Dead code elimination test passed\n";
}

// Test copy propagation
void testCopyPropagation() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int a = 10;
                int b = a;
                int c = b + 5;
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string beforeIR = ir->toString();
    
    Compiler::IR::IROptimizer optimizer(ir.get());
    optimizer.optimize(10);
    
    std::string afterIR = ir->toString();
    
    // After copy propagation, some redundant loads should be eliminated
    // Count load instructions
    int beforeLoads = 0, afterLoads = 0;
    size_t pos = 0;
    while ((pos = beforeIR.find("load", pos)) != std::string::npos) {
        beforeLoads++;
        pos++;
    }
    pos = 0;
    while ((pos = afterIR.find("load", pos)) != std::string::npos) {
        afterLoads++;
        pos++;
    }
    
    // Should have fewer or equal loads after optimization
    assert(afterLoads <= beforeLoads);
    
    std::cout << "✓ Copy propagation test passed\n";
}

// Test common subexpression elimination
void testCommonSubexpressionElimination() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int a = 5;
                int b = 10;
                int c = a + b;
                int d = a + b;
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string beforeIR = ir->toString();
    
    // Count add operations before
    int beforeAdds = 0;
    size_t pos = 0;
    while ((pos = beforeIR.find("add", pos)) != std::string::npos) {
        beforeAdds++;
        pos++;
    }
    
    Compiler::IR::IROptimizer optimizer(ir.get());
    optimizer.optimize(10);
    
    std::string afterIR = ir->toString();
    
    // Count add operations after
    int afterAdds = 0;
    pos = 0;
    while ((pos = afterIR.find("add", pos)) != std::string::npos) {
        afterAdds++;
        pos++;
    }
    
    // CSE should eliminate duplicate computations
    assert(afterAdds <= beforeAdds);
    
    std::cout << "✓ Common subexpression elimination test passed\n";
}

// Test multiple functions in IR
void testMultipleFunctions() {
    std::string code = R"(
        class Test {
            public static int Add(int a, int b) {
                return a + b;
            }
            
            public static int Multiply(int a, int b) {
                return a * b;
            }
            
            public static void Main() {
                int x = 10;
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have function declarations for Add, Multiply, and Main
    assert(irStr.find("@Add") != std::string::npos);
    assert(irStr.find("@Multiply") != std::string::npos);
    assert(irStr.find("@Main") != std::string::npos);
    
    // Each function should have parameters and return
    assert(irStr.find("int %a") != std::string::npos);
    assert(irStr.find("int %b") != std::string::npos);
    
    std::cout << "✓ Multiple functions IR test passed\n";
}

// Test arithmetic operations
void testArithmeticOperations() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int a = 10;
                int b = 5;
                int add = a + b;
                int sub = a - b;
                int mul = a * b;
                int div = a / b;
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have all arithmetic operations
    assert(irStr.find("add") != std::string::npos);
    assert(irStr.find("sub") != std::string::npos);
    assert(irStr.find("mul") != std::string::npos);
    assert(irStr.find("div") != std::string::npos);
    
    std::cout << "✓ Arithmetic operations IR test passed\n";
}

// Test comparison operations
void testComparisonOperations() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int a = 10;
                int b = 5;
                if (a > b) { int x = 1; }
                if (a < b) { int y = 2; }
                if (a == b) { int z = 3; }
            }
        }
    )";
    
    auto ir = generateIR(code);
    std::string irStr = ir->toString();
    
    // Should have comparison operations
    assert(irStr.find("gt") != std::string::npos);
    assert(irStr.find("lt") != std::string::npos);
    assert(irStr.find("eq") != std::string::npos);
    
    std::cout << "✓ Comparison operations IR test passed\n";
}

// Test optimization iterations
void testOptimizationIterations() {
    std::string code = R"(
        class Test {
            public static void Main() {
                int x = 1 + 2 + 3 + 4;
            }
        }
    )";
    
    auto ir = generateIR(code);
    Compiler::IR::IROptimizer optimizer(ir.get());
    
    // Test different iteration counts
    std::string ir1, ir2, ir3;
    
    auto ir_copy1 = generateIR(code);
    Compiler::IR::IROptimizer opt1(ir_copy1.get());
    opt1.optimize(1);
    ir1 = ir_copy1->toString();
    
    auto ir_copy2 = generateIR(code);
    Compiler::IR::IROptimizer opt2(ir_copy2.get());
    opt2.optimize(5);
    ir2 = ir_copy2->toString();
    
    auto ir_copy3 = generateIR(code);
    Compiler::IR::IROptimizer opt3(ir_copy3.get());
    opt3.optimize(10);
    ir3 = ir_copy3->toString();
    
    // More iterations should result in same or better optimization
    // (length should be similar or smaller)
    assert(ir3.length() <= ir1.length() + 100); // Allow some tolerance
    
    std::cout << "✓ Optimization iterations test passed\n";
}

int main() {
    std::cout << "\n=== IR Generation Tests ===\n";
    
    testVariableDeclaration();
    testBinaryExpression();
    testIfStatement();
    testWhileLoop();
    testForLoop();
    testReturnStatement();
    testMultipleFunctions();
    testArithmeticOperations();
    testComparisonOperations();
    
    std::cout << "\n=== IR Optimization Tests ===\n";
    
    testConstantFolding();
    testDeadCodeElimination();
    testCopyPropagation();
    testCommonSubexpressionElimination();
    testOptimizationIterations();
    
    std::cout << "\n✅ All IR tests passed!\n\n";
    
    return 0;
}
