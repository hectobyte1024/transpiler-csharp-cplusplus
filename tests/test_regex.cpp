#include "../src/regex/RegexParser.h"
#include <iostream>
#include <cassert>

void testBasicChar() {
    Regex::RegexEngine regex("a");
    assert(regex.match("a"));
    assert(!regex.match("b"));
    std::cout << "✓ Basic character test passed\n";
}

void testConcatenation() {
    Regex::RegexEngine regex("abc");
    assert(regex.match("abc"));
    assert(!regex.match("ab"));
    assert(!regex.match("abcd"));
    std::cout << "✓ Concatenation test passed\n";
}

void testAlternation() {
    Regex::RegexEngine regex("a|b");
    assert(regex.match("a"));
    assert(regex.match("b"));
    assert(!regex.match("c"));
    std::cout << "✓ Alternation test passed\n";
}

void testKleeneStar() {
    Regex::RegexEngine regex("a*");
    assert(regex.match(""));
    assert(regex.match("a"));
    assert(regex.match("aaa"));
    assert(!regex.match("b"));
    std::cout << "✓ Kleene star test passed\n";
}

void testPlus() {
    Regex::RegexEngine regex("a+");
    assert(!regex.match(""));
    assert(regex.match("a"));
    assert(regex.match("aaa"));
    std::cout << "✓ Plus test passed\n";
}

void testOptional() {
    Regex::RegexEngine regex("ab?c");
    assert(regex.match("ac"));
    assert(regex.match("abc"));
    assert(!regex.match("abbc"));
    std::cout << "✓ Optional test passed\n";
}

void testComplex() {
    Regex::RegexEngine regex("(a|b)*c");
    assert(regex.match("c"));
    assert(regex.match("ac"));
    assert(regex.match("abc"));
    assert(regex.match("ababc"));
    assert(!regex.match("d"));
    std::cout << "✓ Complex pattern test passed\n";
}

void testDigit() {
    Regex::RegexEngine regex("\\d+");
    assert(regex.match("123"));
    assert(regex.match("0"));
    assert(!regex.match("abc"));
    std::cout << "✓ Digit pattern test passed\n";
}

int main() {
    std::cout << "Running Regex Engine Tests...\n\n";
    
    testBasicChar();
    testConcatenation();
    testAlternation();
    testKleeneStar();
    testPlus();
    testOptional();
    testComplex();
    testDigit();
    
    std::cout << "\n✓ All regex tests passed!\n";
    return 0;
}
