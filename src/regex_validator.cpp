#include "regex/RegexParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

struct TestCase {
    std::string input;
    bool expectedResult;
    std::string description;
};

struct RegexPattern {
    std::string pattern;
    std::string description;
    std::vector<TestCase> tests;
};

// Parse test file format:
// PATTERN: regex_pattern
// DESC: description
// TESTS:
//   input → expected_result
//   input → expected_result
// ---
std::vector<RegexPattern> parseTestFile(const std::string& filename) {
    std::vector<RegexPattern> patterns;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::string line;
    RegexPattern current;
    bool inTests = false;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;  // Skip empty lines and comments
        }
        
        if (line == "---") {
            if (!current.pattern.empty()) {
                patterns.push_back(current);
                current = RegexPattern();
                inTests = false;
            }
            continue;
        }
        
        if (line.find("PATTERN:") == 0) {
            current.pattern = line.substr(8);
            current.pattern.erase(0, current.pattern.find_first_not_of(" \t"));
            inTests = false;
            continue;
        }
        
        if (line.find("DESC:") == 0) {
            current.description = line.substr(5);
            current.description.erase(0, current.description.find_first_not_of(" \t"));
            continue;
        }
        
        if (line == "TESTS:") {
            inTests = true;
            continue;
        }
        
        if (inTests && line.find("->") != std::string::npos) {
            // Parse test case: "input -> expected"
            size_t arrowPos = line.find("->");
            std::string input = line.substr(0, arrowPos);
            std::string expected = line.substr(arrowPos + 2);
            
            // Trim
            input.erase(input.find_last_not_of(" \t") + 1);
            expected.erase(0, expected.find_first_not_of(" \t"));
            
            // Handle special escape sequences in input
            std::string processedInput;
            for (size_t i = 0; i < input.length(); i++) {
                if (input[i] == '\\' && i + 1 < input.length()) {
                    char next = input[i + 1];
                    if (next == 'n') {
                        processedInput += '\n';
                        i++;
                    } else if (next == 't') {
                        processedInput += '\t';
                        i++;
                    } else if (next == 'r') {
                        processedInput += '\r';
                        i++;
                    } else {
                        processedInput += input[i];
                    }
                } else {
                    processedInput += input[i];
                }
            }
            
            bool expectedResult = (expected == "true");
            TestCase test{processedInput, expectedResult, ""};
            current.tests.push_back(test);
        }
    }
    
    if (!current.pattern.empty()) {
        patterns.push_back(current);
    }
    
    return patterns;
}

void printHeader() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║        INTERACTIVE REGEX VALIDATOR & TEST RUNNER          ║\n";
    std::cout << "║   Validate regular expressions with detailed reports      ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
}

void testPattern(const RegexPattern& pattern) {
    std::cout << "┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│ PATTERN: " << std::setw(48) << std::left << pattern.pattern << "│\n";
    
    if (!pattern.description.empty()) {
        std::cout << "│ DESC:    " << std::setw(48) << std::left << pattern.description << "│\n";
    }
    
    std::cout << "└─────────────────────────────────────────────────────────┘\n";
    
    int passed = 0;
    int failed = 0;
    std::vector<std::string> failedTests;
    
    try {
        Regex::RegexEngine regex(pattern.pattern);
        
        std::cout << "\n  Test Results:\n";
        std::cout << "  " << std::string(55, '─') << "\n";
        
        int testNum = 0;
        for (const auto& test : pattern.tests) {
            testNum++;
            bool result = regex.match(test.input);
            bool passed_test = (result == test.expectedResult);
            
            std::string status = passed_test ? "✓ PASS" : "✗ FAIL";
            std::string input_display = test.input.empty() ? "(empty)" : test.input;
            
            // Limit display length
            if (input_display.length() > 20) {
                input_display = input_display.substr(0, 17) + "...";
            }
            
            std::cout << "  " << std::setw(3) << testNum << ". "
                     << std::setw(20) << std::left << input_display
                     << " → " << (test.expectedResult ? "true" : "false")
                     << " ... " << status << "\n";
            
            if (passed_test) {
                passed++;
            } else {
                failed++;
                failedTests.push_back("Test " + std::to_string(testNum) + ": '" + input_display +
                                    "' expected " + (test.expectedResult ? "true" : "false") +
                                    " but got " + (result ? "true" : "false"));
            }
        }
        
        std::cout << "  " << std::string(55, '─') << "\n";
        
        // Summary
        double passRate = pattern.tests.empty() ? 0 : (100.0 * passed / pattern.tests.size());
        std::cout << "\n  Summary: " << passed << "/" << pattern.tests.size()
                 << " tests passed (" << std::fixed << std::setprecision(1) << passRate << "%)\n";
        
        if (!failedTests.empty()) {
            std::cout << "\n  Failed Tests:\n";
            for (const auto& failMsg : failedTests) {
                std::cout << "    • " << failMsg << "\n";
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "  ✗ ERROR: " << e.what() << "\n";
        std::cout << "  Compilation failed for pattern: " << pattern.pattern << "\n";
    }
    
    std::cout << "\n";
}

void printStatistics(const std::vector<RegexPattern>& patterns) {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    OVERALL STATISTICS                     ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════╣\n";
    
    int totalPatterns = patterns.size();
    int totalTests = 0;
    int passedPatterns = 0;
    int totalPassed = 0;
    int totalFailed = 0;
    
    for (const auto& pattern : patterns) {
        totalTests += pattern.tests.size();
        
        try {
            Regex::RegexEngine regex(pattern.pattern);
            int patternPassed = 0;
            
            for (const auto& test : pattern.tests) {
                bool result = regex.match(test.input);
                if (result == test.expectedResult) {
                    patternPassed++;
                    totalPassed++;
                } else {
                    totalFailed++;
                }
            }
            
            if (patternPassed == pattern.tests.size()) {
                passedPatterns++;
            }
        } catch (...) {
            totalFailed += pattern.tests.size();
        }
    }
    
    double overallRate = totalTests == 0 ? 0 : (100.0 * totalPassed / totalTests);
    double patternRate = totalPatterns == 0 ? 0 : (100.0 * passedPatterns / totalPatterns);
    
    std::cout << "│ Total Patterns:        " << std::setw(34) << std::left << totalPatterns << "│\n";
    std::cout << "│ Patterns 100% Pass:    " << std::setw(34) << std::left << passedPatterns << "│\n";
    std::cout << "│ Pattern Pass Rate:     " << std::setw(31) << std::left << std::fixed
             << std::setprecision(1) << patternRate << "%  │\n";
    std::cout << "│                                                           │\n";
    std::cout << "│ Total Test Cases:      " << std::setw(34) << std::left << totalTests << "│\n";
    std::cout << "│ Passed Tests:          " << std::setw(34) << std::left << totalPassed << "│\n";
    std::cout << "│ Failed Tests:          " << std::setw(34) << std::left << totalFailed << "│\n";
    std::cout << "│ Overall Pass Rate:     " << std::setw(31) << std::left << std::fixed
             << std::setprecision(1) << overallRate << "%  │\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
}

int main(int argc, char* argv[]) {
    printHeader();
    
    std::string filename;
    bool verbose = false;
    
    // Parse command line arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <testfile.txt> [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -v, --verbose   Show detailed output for each test\n";
        std::cout << "\nExample test file format:\n";
        std::cout << "  PATTERN: [a-z]+\n";
        std::cout << "  DESC: Lowercase letters\n";
        std::cout << "  TESTS:\n";
        std::cout << "    hello → true\n";
        std::cout << "    HELLO → false\n";
        std::cout << "    123 → false\n";
        std::cout << "  ---\n";
        std::cout << "  PATTERN: \\d+\n";
        std::cout << "  DESC: One or more digits\n";
        std::cout << "  TESTS:\n";
        std::cout << "    123 → true\n";
        std::cout << "    abc → false\n";
        std::cout << "  ---\n";
        return 0;
    }
    
    filename = argv[1];
    
    // Check for verbose flag
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
    }
    
    try {
        std::cout << "Reading test file: " << filename << "\n\n";
        
        auto patterns = parseTestFile(filename);
        
        if (patterns.empty()) {
            std::cout << "Warning: No test patterns found in file.\n";
            return 1;
        }
        
        std::cout << "Found " << patterns.size() << " pattern(s) to test.\n\n";
        std::cout << std::string(60, '=') << "\n\n";
        
        // Test each pattern
        int patternNum = 0;
        for (const auto& pattern : patterns) {
            patternNum++;
            std::cout << "[" << patternNum << "/" << patterns.size() << "] ";
            testPattern(pattern);
        }
        
        std::cout << std::string(60, '=') << "\n\n";
        
        // Print overall statistics
        printStatistics(patterns);
        
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
