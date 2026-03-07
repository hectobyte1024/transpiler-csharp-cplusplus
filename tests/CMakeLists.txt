cmake_minimum_required(VERSION 3.10)

# Test for regex engine
add_executable(test_regex test_regex.cpp
    ../src/regex/NFA.cpp
    ../src/regex/DFA.cpp
    ../src/regex/RegexParser.cpp
)

target_include_directories(test_regex PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test for lexer
add_executable(test_lexer test_lexer.cpp
    ../src/regex/NFA.cpp
    ../src/regex/DFA.cpp
    ../src/regex/RegexParser.cpp
    ../src/lexer/Token.cpp
    ../src/lexer/Lexer.cpp
)

target_include_directories(test_lexer PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test for parser
add_executable(test_parser test_parser.cpp
    ../src/regex/NFA.cpp
    ../src/regex/DFA.cpp
    ../src/regex/RegexParser.cpp
    ../src/lexer/Token.cpp
    ../src/lexer/Lexer.cpp
    ../src/parser/AST.cpp
    ../src/parser/Parser.cpp
)

target_include_directories(test_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test for IR
add_executable(test_ir test_ir.cpp
    ../src/regex/NFA.cpp
    ../src/regex/DFA.cpp
    ../src/regex/RegexParser.cpp
    ../src/lexer/Token.cpp
    ../src/lexer/Lexer.cpp
    ../src/parser/AST.cpp
    ../src/parser/Parser.cpp
    ../src/ir/IR.cpp
    ../src/ir/IRGenerator.cpp
    ../src/ir/IROptimizer.cpp
)

target_include_directories(test_ir PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Add tests
enable_testing()

add_test(NAME regex_test COMMAND test_regex)
add_test(NAME lexer_test COMMAND test_lexer)
add_test(NAME parser_test COMMAND test_parser)
add_test(NAME ir_test COMMAND test_ir)
