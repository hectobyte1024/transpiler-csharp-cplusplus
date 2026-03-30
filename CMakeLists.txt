cmake_minimum_required(VERSION 3.10)
project(CSharpToCppCompiler VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files
set(REGEX_SOURCES
    src/regex/NFA.cpp
    src/regex/DFA.cpp
    src/regex/RegexParser.cpp
)

set(LEXER_SOURCES
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp
)

set(PARSER_SOURCES
    src/parser/AST.cpp
    src/parser/Parser.cpp
)

set(CODEGEN_SOURCES
    src/codegen/CodeGenerator.cpp
)

# Main executable
add_executable(cs2cpp
    src/main.cpp
    ${REGEX_SOURCES}
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${CODEGEN_SOURCES}
)

# Regex validator executable
add_executable(regex_validator
    src/regex_validator.cpp
    ${REGEX_SOURCES}
)

# Regex tracer/debugger executable
add_executable(regex_trace
    src/regex_trace.cpp
    ${REGEX_SOURCES}
)

# Include directories
target_include_directories(cs2cpp PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(regex_validator PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(regex_trace PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Install target
install(TARGETS cs2cpp DESTINATION bin)

# Optional: Build tests
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
