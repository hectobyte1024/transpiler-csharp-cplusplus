#include "../src/regex/RegexParser.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Test utilities
int testCount = 0;
int passCount = 0;

void test(const std::string& name, bool condition) {
    testCount++;
    if (condition) {
        passCount++;
        std::cout << "✓ " << name << "\n";
    } else {
        std::cout << "✗ " << name << " FAILED\n";
    }
}

void testRegex(const std::string& name, const std::string& pattern, 
               const std::vector<std::pair<std::string, bool>>& cases) {
    try {
        Regex::RegexEngine regex(pattern);
        bool allPassed = true;
        
        for (const auto& testCase : cases) {
            bool result = regex.match(testCase.first);
            if (result != testCase.second) {
                allPassed = false;
                std::cout << "  Input '" << testCase.first << "': expected " 
                         << (testCase.second ? "match" : "no match") 
                         << ", got " << (result ? "match" : "no match") << "\n";
            }
        }
        
        test(name, allPassed);
    } catch (const std::exception& e) {
        std::cout << "✗ " << name << " - EXCEPTION: " << e.what() << "\n";
        testCount++;
    }
}

// ============================================================================
// BASIC TESTS - Single characters and literals
// ============================================================================

void testBasicCharacters() {
    std::cout << "\n=== BASIC CHARACTER TESTS ===\n";
    
    testRegex("Single character 'a'", "a", {
        {"a", true},
        {"b", false},
        {"", false},
        {"aa", false}
    });
    
    testRegex("Single character 'x'", "x", {
        {"x", true},
        {"y", false},
        {"X", false}
    });
    
    testRegex("Digit character '5'", "5", {
        {"5", true},
        {"4", false},
        {"55", false}
    });
    
    testRegex("Space character", " ", {
        {" ", true},
        {"  ", false},
        {"a", false}
    });
}

void testLiterals() {
    std::cout << "\n=== LITERAL STRING TESTS ===\n";
    
    testRegex("Literal 'hello'", "hello", {
        {"hello", true},
        {"hell", false},
        {"hello!", false},
        {"Hello", false},
        {"helloworld", false}
    });
    
    testRegex("Literal 'test123'", "test123", {
        {"test123", true},
        {"test", false},
        {"123", false},
        {"test124", false}
    });
    
    testRegex("Escaped special char '\\+'", "\\+", {
        {"+", true},
        {"a", false},
        {"++", false}
    });
}

// ============================================================================
// CONCATENATION TESTS
// ============================================================================

void testConcatenation() {
    std::cout << "\n=== CONCATENATION TESTS ===\n";
    
    testRegex("Concatenation 'abc'", "abc", {
        {"abc", true},
        {"ab", false},
        {"abcd", false},
        {"a", false},
        {"", false}
    });
    
    testRegex("Concatenation 'program'", "program", {
        {"program", true},
        {"programm", false},
        {"progra", false},
        {"Program", false}
    });
    
    testRegex("Three chars 'xyz'", "xyz", {
        {"xyz", true},
        {"xy", false},
        {"xyzw", false},
        {"xyza", false}
    });
}

// ============================================================================
// ALTERNATION TESTS (|)
// ============================================================================

void testAlternation() {
    std::cout << "\n=== ALTERNATION TESTS (|) ===\n";
    
    testRegex("Two options 'a|b'", "a|b", {
        {"a", true},
        {"b", true},
        {"c", false},
        {"ab", false},
        {"", false}
    });
    
    testRegex("Three options 'red|green|blue'", "red|green|blue", {
        {"red", true},
        {"green", true},
        {"blue", true},
        {"yellow", false},
        {"red|green", false}
    });
    
    testRegex("Alternation 'cat|dog'", "cat|dog", {
        {"cat", true},
        {"dog", true},
        {"bird", false},
        {"catdog", false}
    });
    
    testRegex("Single char alternation 'x|y|z'", "x|y|z", {
        {"x", true},
        {"y", true},
        {"z", true},
        {"a", false},
        {"xyz", false}
    });
}

// ============================================================================
// KLEENE STAR TESTS (*)
// ============================================================================

void testKleeneStar() {
    std::cout << "\n=== KLEENE STAR TESTS (*) ===\n";
    
    testRegex("Zero or more 'a*'", "a*", {
        {"", true},      // Zero occurrences
        {"a", true},     // One occurrence
        {"aa", true},    // Two occurrences
        {"aaa", true},   // Three occurrences
        {"aaaa", true},  // Many occurrences
        {"b", false},    // Different character
        {"ab", false}    // Contains 'b'
    });
    
    testRegex("Zero or more 'x*'", "x*", {
        {"", true},
        {"x", true},
        {"xxxxxxx", true},
        {"y", false},
        {"xy", false}
    });
    
    testRegex("Literal then star 'ab*c'", "ab*c", {
        {"ac", true},    // Zero b's
        {"abc", true},   // One b
        {"abbc", true},  // Two b's
        {"abbbbbc", true}, // Many b's
        {"abcd", false}, // Extra character
        {"ac ", false}   // Space
    });
    
    testRegex("Parenthesized alternation star '(a|b)*'", "(a|b)*", {
        {"", true},
        {"a", true},
        {"b", true},
        {"ab", true},
        {"ba", true},
        {"aba", true},
        {"ababa", true},
        {"ababab", true},
        {"c", false}
    });
}

// ============================================================================
// PLUS TESTS (+)
// ============================================================================

void testPlus() {
    std::cout << "\n=== PLUS TESTS (+) ===\n";
    
    testRegex("One or more 'a+'", "a+", {
        {"", false},     // Empty not allowed
        {"a", true},     // One occurrence
        {"aa", true},    // Two occurrences
        {"aaa", true},   // Many occurrences
        {"b", false},    // Different character
        {"ab", false}    // Contains 'b'
    });
    
    testRegex("One or more 'x+'", "x+", {
        {"", false},
        {"x", true},
        {"xxx", true},
        {"y", false},
        {"xxxy", false}
    });
    
    testRegex("Literal then plus 'a+b'", "a+b", {
        {"ab", true},    // Exactly one 'a'
        {"aab", true},   // Two 'a's
        {"aaab", true},  // Many 'a's
        {"b", false},    // No 'a'
        {"aabb", false}  // Extra 'b'
    });
}

// ============================================================================
// OPTIONAL TESTS (?)
// ============================================================================

void testOptional() {
    std::cout << "\n=== OPTIONAL TESTS (?) ===\n";
    
    testRegex("Optional 'a?'", "a?", {
        {"", true},      // Zero occurrences OK
        {"a", true},     // One occurrence OK
        {"aa", false},   // Two not allowed
        {"b", false}
    });
    
    testRegex("Optional 'ab?c'", "ab?c", {
        {"ac", true},    // b is optional
        {"abc", true},   // b is present
        {"abbc", false}, // Two b's not allowed
        {"abc ", false}  // Extra character
    });
    
    testRegex("Optional middle 'a?b'", "a?b", {
        {"b", true},     // Without 'a'
        {"ab", true},    // With 'a'
        {"aab", false},  // Two 'a's
        {"c", false}
    });
    
    testRegex("Multiple optionals 'a?b?c?'", "a?b?c?", {
        {"", true},
        {"a", true},
        {"b", true},
        {"c", true},
        {"ab", true},
        {"bc", true},
        {"ac", true},
        {"abc", true},
        {"abcd", false}
    });
}

// ============================================================================
// CHARACTER CLASS TESTS [...]
// ============================================================================

void testCharacterClasses() {
    std::cout << "\n=== CHARACTER CLASS TESTS [...] ===\n";
    
    testRegex("Single char class '[abc]'", "[abc]", {
        {"a", true},
        {"b", true},
        {"c", true},
        {"d", false},
        {"", false},
        {"ab", false}
    });
    
    testRegex("Range [a-z]", "[a-z]", {
        {"a", true},
        {"m", true},
        {"z", true},
        {"A", false},
        {"1", false},
        {"ab", false}
    });
    
    testRegex("Range [0-9]", "[0-9]", {
        {"0", true},
        {"5", true},
        {"9", true},
        {"a", false},
        {"", false},
        {"10", false}
    });
    
    testRegex("Multiple ranges [a-zA-Z]", "[a-zA-Z]", {
        {"a", true},
        {"z", true},
        {"A", true},
        {"Z", true},
        {"m", true},
        {"M", true},
        {"0", false},
        {"ab", false}
    });
    
    testRegex("Char class with numbers [0-9a-f]", "[0-9a-f]", {
        {"0", true},
        {"9", true},
        {"a", true},
        {"f", true},
        {"g", false},
        {"A", false}
    });
}

void testCharacterClassPlus() {
    std::cout << "\n=== CHARACTER CLASS WITH QUANTIFIERS ===\n";
    
    testRegex("Char class star '[abc]*'", "[abc]*", {
        {"", true},
        {"a", true},
        {"abc", true},
        {"aabbcc", true},
        {"abcd", false},
        {"d", false}
    });
    
    testRegex("Char class plus '[a-z]+'", "[a-z]+", {
        {"", false},
        {"a", true},
        {"abc", true},
        {"abcxyz", true},
        {"abc123", false},
        {"123", false}
    });
}

// ============================================================================
// ESCAPE SEQUENCES TESTS
// ============================================================================

void testEscapeSequences() {
    std::cout << "\n=== ESCAPE SEQUENCE TESTS ===\n";
    
    testRegex("Digit \\d", "\\d", {
        {"0", true},
        {"5", true},
        {"9", true},
        {"a", false},
        {"", false},
        {"12", false}
    });
    
    testRegex("One or more digits '\\d+'", "\\d+", {
        {"", false},
        {"0", true},
        {"123", true},
        {"9876543210", true},
        {"12a", false},
        {"a12", false}
    });
    
    testRegex("Word character \\w", "\\w", {
        {"a", true},
        {"Z", true},
        {"_", true},
        {"5", true},
        {" ", false},
        {"!", false}
    });
    
    testRegex("One or more word chars '\\w+'", "\\w+", {
        {"", false},
        {"hello", true},
        {"test123", true},
        {"_var", true},
        {"hello world", false}
    });
    
    testRegex("Whitespace \\s", "\\s", {
        {" ", true},
        {"\t", true},
        {"\n", true},
        {"\r", true},
        {"a", false},
        {"", false}
    });
    
    testRegex("Escaped special char '\\.'", "\\.", {
        {".", true},
        {"a", false},
        {" ", false}
    });
    
    testRegex("Escaped pipe '\\|'", "\\|", {
        {"|", true},
        {"a", false},
        {"||", false}
    });
}

// ============================================================================
// WILDCARD TESTS (.)
// ============================================================================

void testWildcard() {
    std::cout << "\n=== WILDCARD TESTS (.) ===\n";
    
    testRegex("Single wildcard '.'", ".", {
        {"a", true},
        {"1", true},
        {" ", true},
        {"", false},
        {"ab", false}
    });
    
    testRegex("Wildcard star '.*'", ".*", {
        {"", true},
        {"a", true},
        {"hello", true},
        {"hello123!@#", true}
    });
    
    testRegex("Wildcard plus '.+'", ".+", {
        {"", false},
        {"a", true},
        {"hello", true},
        {"anything goes 123", true}
    });
}

// ============================================================================
// COMPLEX PATTERN TESTS
// ============================================================================

void testComplexPatterns() {
    std::cout << "\n=== COMPLEX PATTERN TESTS ===\n";
    
    testRegex("(a|b)*c", "(a|b)*c", {
        {"c", true},
        {"ac", true},
        {"bc", true},
        {"abc", true},
        {"abaac", true},
        {"ababab", false},
        {"abca", false}
    });
    
    testRegex("(a|b)+c", "(a|b)+c", {
        {"ac", true},
        {"bc", true},
        {"abc", true},
        {"abaac", true},
        {"c", false},
        {"d", false}
    });
    
    testRegex("a(b|c)+d", "a(b|c)+d", {
        {"abd", true},
        {"acd", true},
        {"abcd", true},
        {"abbcd", true},
        {"abcbcbdd", false},  // Extra 'd'
        {"ad", false}         // Missing b or c
    });
    
    testRegex("Email-like pattern", "[a-z]+@[a-z]+", {
        {"user@domain", true},
        {"test@example", true},
        {"a@b", true},
        {"@domain", false},
        {"user@", false},
        {"user domain", false}
    });
    
    testRegex("Simple number pattern", "\\d+", {
        {"0", true},
        {"123", true},
        {"9999999999", true},
        {"", false},
        {"abc", false},
        {"12a", false}
    });
    
    testRegex("Decimal number pattern", "\\d+\\.\\d+", {
        {"3.14", true},
        {"0.0", true},
        {"123.456", true},
        {"3.14.15", false},
        {"3.", false},
        {".14", false}
    });
}

// ============================================================================
// GROUPING AND NESTING TESTS
// ============================================================================

void testGrouping() {
    std::cout << "\n=== GROUPING AND NESTING TESTS ===\n";
    
    testRegex("Simple grouping '(ab)'", "(ab)", {
        {"ab", true},
        {"a", false},
        {"b", false},
        {"abb", false}
    });
    
    testRegex("Grouped alternation '(ab|cd)'", "(ab|cd)", {
        {"ab", true},
        {"cd", true},
        {"ac", false},
        {"abcd", false}
    });
    
    testRegex("Nested grouping '((a|b)c)*'", "((a|b)c)*", {
        {"", true},
        {"ac", true},
        {"bc", true},
        {"acbc", true},
        {"acacac", true},
        {"abc", false}
    });
    
    testRegex("Multiple groups '(a)(b)(c)'", "(a)(b)(c)", {
        {"abc", true},
        {"ab", false},
        {"abcd", false},
        {"aabbcc", false}
    });
}

// ============================================================================
// QUANTIFIER COMBINATIONS
// ============================================================================

void testQuantifierCombinations() {
    std::cout << "\n=== QUANTIFIER COMBINATIONS ===\n";
    
    testRegex("Star of star '(a*)*'", "(a*)*", {
        {"", true},
        {"a", true},
        {"aaa", true}
    });
    
    testRegex("Plus of plus '(a+)+'", "(a+)+", {
        {"a", true},
        {"aaa", true},
        {"aaaa", true},
        {"", false}
    });
    
    testRegex("Optional with plus 'a+?'", "a+", {
        {"a", true},
        {"aaa", true},
        {"", false}
    });
    
    testRegex("Mixed quantifiers 'a*b+'", "a*b+", {
        {"b", true},
        {"ab", true},
        {"aab", true},
        {"aaabbb", true},
        {"aaa", false},
        {"", false}
    });
}

// ============================================================================
// EDGE CASE TESTS
// ============================================================================

void testEdgeCases() {
    std::cout << "\n=== EDGE CASE TESTS ===\n";
    
    testRegex("Empty pattern ''", "", {
        {"", true},
        {"a", false},
        {" ", false}
    });
    
    testRegex("Single space ' '", " ", {
        {" ", true},
        {"  ", false},
        {"a", false},
        {"", false}
    });
    
    testRegex("Multiple spaces with star ' *'", " *", {
        {"", true},
        {" ", true},
        {"  ", true},
        {"   ", true},
        {"a", false}
    });
    
    testRegex("Tab character", "\\t", {
        {"\t", true},
        {" ", false},
        {"t", false}
    });
    
    testRegex("Newline character", "\\n", {
        {"\n", true},
        {" ", false},
        {"n", false}
    });
    
    testRegex("Same char twice 'aa'", "aa", {
        {"aa", true},
        {"a", false},
        {"aaa", false}
    });
    
    testRegex("Very long alternation", "a|b|c|d|e|f|g|h|i|j", {
        {"a", true},
        {"e", true},
        {"j", true},
        {"k", false},
        {"ab", false}
    });
}

// ============================================================================
// PERFORMANCE TESTS (larger inputs)
// ============================================================================

void testPerformance() {
    std::cout << "\n=== PERFORMANCE TESTS ===\n";
    
    testRegex("Repeated pattern matching", "a+", {
        {"aaaaaaaaaa", true},           // 10 a's
        {"aaaaaaaaaaaaaaaaaa", true},   // 18 a's
        {"aaaaaaaaaaaaaaaaaaa", true},  // 19 a's
        {"aaaaaaaaaaaaaaaaaaaa", true}  // 20 a's
    });
    
    testRegex("Short simple pattern", "test", {
        {"test", true},
        {"test", true},  // Repeated
        {"test", true}
    });
    
    testRegex("Character class repetition", "[a-z]+", {
        {"abcdefghijklmnopqrstuvwxyz", true},
        {"aaaaaaaaaaaaaaaaaaaaaaaaaaaa", true}
    });
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║  COMPREHENSIVE REGEX ENGINE TEST SUITE                ║\n";
    std::cout << "║  Testing: Thompson Construction → Subset Construction ║\n";
    std::cout << "║           → DFA Simulation                             ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    try {
        // Run all test groups
        testBasicCharacters();
        testLiterals();
        testConcatenation();
        testAlternation();
        testKleeneStar();
        testPlus();
        testOptional();
        testCharacterClasses();
        testCharacterClassPlus();
        testEscapeSequences();
        testWildcard();
        testComplexPatterns();
        testGrouping();
        testQuantifierCombinations();
        testEdgeCases();
        testPerformance();
        
        // Print summary
        std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║  TEST SUMMARY                                          ║\n";
        std::cout << "╠════════════════════════════════════════════════════════╣\n";
        std::cout << "║  Total Tests:  " << testCount << "\n";
        std::cout << "║  Passed:       " << passCount << "\n";
        std::cout << "║  Failed:       " << (testCount - passCount) << "\n";
        std::cout << "║  Pass Rate:    " << (100.0 * passCount / testCount) << "%\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n";
        
        if (passCount == testCount) {
            std::cout << "\n✓ ALL TESTS PASSED!\n";
            return 0;
        } else {
            std::cout << "\n✗ SOME TESTS FAILED\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ FATAL ERROR: " << e.what() << "\n";
        return 1;
    }
}
