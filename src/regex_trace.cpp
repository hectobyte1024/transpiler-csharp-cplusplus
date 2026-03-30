#include "regex/RegexParser.h"
#include <iostream>
#include <string>
#include <iomanip>

void printWelcome() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║         INTERACTIVE REGEX TRACER & DEBUGGER               ║\n";
    std::cout << "║  Watch your regex engine match characters step-by-step     ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
}

void printHelp() {
    std::cout << "\n┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│ COMMANDS:                                               │\n";
    std::cout << "│   pattern <regex>    - Set the regex pattern to test    │\n";
    std::cout << "│   input <string>     - Set the input string to match    │\n";
    std::cout << "│   trace              - Show character-by-character      │\n";
    std::cout << "│   match              - Quick match result               │\n";
    std::cout << "│   debug              - Detailed trace with state info   │\n";
    std::cout << "│   help               - Show this help                   │\n";
    std::cout << "│   quit               - Exit the program                 │\n";
    std::cout << "│                                                         │\n";
    std::cout << "│ EXAMPLE:                                                │\n";
    std::cout << "│   > pattern [a-z]+                                      │\n";
    std::cout << "│   > input hello                                         │\n";
    std::cout << "│   > trace                                               │\n";
    std::cout << "└─────────────────────────────────────────────────────────┘\n";
}

void tracePattern(const std::string& pattern, const std::string& input) {
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║ CHARACTER-BY-CHARACTER TRACE                              ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════╣\n";
    std::cout << "│ Pattern: " << std::setw(51) << std::left << pattern << "│\n";
    std::cout << "│ Input:   " << std::setw(51) << std::left 
             << (input.empty() ? "(empty string)" : input) << "│\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
    
    try {
        Regex::RegexEngine regex(pattern);
        
        // Simulate step-by-step
        const Regex::DFA& dfa = regex.getDFA();
        Regex::DFAState* currentState = dfa.getStartState();
        
        if (!currentState) {
            std::cout << "Error: No start state in DFA\n";
            return;
        }
        
        std::cout << "Start State: " << currentState->id 
                 << (currentState->isAccepting ? " (ACCEPTING)" : "") << "\n\n";
        
        int charIndex = 0;
        bool matched = true;
        
        if (input.empty()) {
            if (currentState->isAccepting) {
                std::cout << "✓ MATCH: Pattern matches empty string\n";
            } else {
                std::cout << "✗ NO MATCH: Pattern does not match empty string\n";
            }
        } else {
            // Process each character
            bool prematureEnd = false;
            for (char c : input) {
                std::string charDisplay;
                
                // Display special characters nicely
                if (c == ' ') {
                    charDisplay = "(space)";
                } else if (c == '\t') {
                    charDisplay = "(tab)";
                } else if (c == '\n') {
                    charDisplay = "(newline)";
                } else if (c == '\r') {
                    charDisplay = "(carriage return)";
                } else {
                    charDisplay = std::string(1, c);
                }
                
                // Find transition
                auto it = currentState->transitions.find(c);
                
                if (it == currentState->transitions.end()) {
                    // No transition found
                    std::cout << "[" << std::setw(2) << charIndex << "] '" << std::setw(20) 
                             << std::left << charDisplay << "' → "
                             << "NO TRANSITION ✗ FAIL\n";
                    matched = false;
                    prematureEnd = true;
                    break;
                }
                
                Regex::DFAState* nextState = it->second;
                std::cout << "[" << std::setw(2) << charIndex << "] '" << std::setw(20) 
                         << std::left << charDisplay << "' → State " << nextState->id;
                
                if (nextState->isAccepting) {
                    std::cout << " (ACCEPTING)";
                }
                std::cout << " ✓\n";
                
                currentState = nextState;
                charIndex++;
            }
            
            // Check if we're in an accepting state
            std::cout << "\nFinal State: " << currentState->id;
            if (currentState->isAccepting) {
                std::cout << " (ACCEPTING)\n";
            } else {
                std::cout << " (NOT ACCEPTING)\n";
            }
            
            if (prematureEnd) {
                std::cout << "\n✗ NO MATCH: Could not process entire input\n";
                matched = false;
            } else if (currentState->isAccepting) {
                std::cout << "\n✓ MATCH: Pattern successfully matched the input\n";
                matched = true;
            } else {
                std::cout << "\n✗ NO MATCH: Final state is not accepting\n";
                matched = false;
            }
        }
        
        std::cout << "\n" << std::string(60, '─') << "\n";
        std::cout << "Result: " << (matched ? "✓ MATCH" : "✗ NO MATCH") << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "✗ ERROR: " << e.what() << "\n";
    }
}

void debugPattern(const std::string& pattern, const std::string& input) {
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║ DETAILED DEBUG TRACE WITH STATE INFORMATION               ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════╣\n";
    std::cout << "│ Pattern: " << std::setw(51) << std::left << pattern << "│\n";
    std::cout << "│ Input:   " << std::setw(51) << std::left 
             << (input.empty() ? "(empty string)" : input) << "│\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
    
    try {
        Regex::RegexEngine regex(pattern);
        
        const Regex::DFA& dfa = regex.getDFA();
        Regex::DFAState* currentState = dfa.getStartState();
        
        if (!currentState) {
            std::cout << "Error: No start state in DFA\n";
            return;
        }
        
        std::cout << "DFA Start State: " << currentState->id << "\n";
        std::cout << "DFA Total States: " << dfa.getStates().size() << "\n";
        std::cout << "DFA Accept States: " << dfa.getAcceptStates().size() << "\n";
        std::cout << "\n" << std::string(60, '─') << "\n\n";
        
        if (input.empty()) {
            std::cout << "Processing: (empty input)\n";
            if (currentState->isAccepting) {
                std::cout << "✓ MATCH: Start state is accepting\n";
            } else {
                std::cout << "✗ NO MATCH: Start state is not accepting\n";
            }
        } else {
            std::cout << "Processing input character by character:\n\n";
            
            int charIndex = 0;
            
            for (char c : input) {
                std::string charDisplay;
                if (c == ' ') charDisplay = "(space)";
                else if (c == '\t') charDisplay = "(tab)";
                else if (c == '\n') charDisplay = "(newline)";
                else if (c == '\r') charDisplay = "(carriage return)";
                else charDisplay = std::string(1, c);
                
                std::cout << "Step " << charIndex << ":\n";
                std::cout << "  Current State: " << currentState->id << "\n";
                std::cout << "  Input Char:    '" << charDisplay << "' (code: " 
                         << (int)c << ")\n";
                std::cout << "  Transitions:   " << currentState->transitions.size() << "\n";
                
                auto it = currentState->transitions.find(c);
                
                if (it == currentState->transitions.end()) {
                    std::cout << "  Status:        NO TRANSITION FOUND ✗\n";
                    std::cout << "\n✗ MATCH FAILED at position " << charIndex << "\n";
                    return;
                }
                
                Regex::DFAState* nextState = it->second;
                std::cout << "  Next State:    " << nextState->id;
                if (nextState->isAccepting) std::cout << " (ACCEPTING)";
                std::cout << " ✓\n\n";
                
                currentState = nextState;
                charIndex++;
            }
            
            std::cout << "Final Step:\n";
            std::cout << "  Current State: " << currentState->id << "\n";
            std::cout << "  Is Accepting:  " << (currentState->isAccepting ? "YES ✓" : "NO ✗") << "\n";
            std::cout << "\n";
            
            if (currentState->isAccepting) {
                std::cout << "✓ MATCH SUCCESSFUL\n";
            } else {
                std::cout << "✗ MATCH FAILED: Final state is not accepting\n";
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "✗ ERROR: " << e.what() << "\n";
    }
}

void quickMatch(const std::string& pattern, const std::string& input) {
    try {
        Regex::RegexEngine regex(pattern);
        bool result = regex.match(input);
        
        std::string inputDisplay = input.empty() ? "(empty)" : input;
        std::cout << "\nPattern: " << pattern << "\n";
        std::cout << "Input:   " << inputDisplay << "\n";
        std::cout << "Result:  " << (result ? "✓ MATCH" : "✗ NO MATCH") << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "✗ ERROR: " << e.what() << "\n";
    }
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    
    if (start == std::string::npos) return "";
    return str.substr(start, end - start + 1);
}

int main() {
    printWelcome();
    printHelp();
    
    std::string pattern;
    std::string input;
    std::string line;
    
    std::cout << "\n> ";
    
    while (std::getline(std::cin, line)) {
        line = trim(line);
        
        if (line.empty()) {
            std::cout << "> ";
            continue;
        }
        
        if (line == "quit" || line == "exit") {
            std::cout << "\nGoodbye!\n";
            break;
        }
        
        if (line == "help") {
            printHelp();
            std::cout << "> ";
            continue;
        }
        
        if (line.substr(0, 7) == "pattern") {
            pattern = trim(line.substr(7));
            if (pattern.empty()) {
                std::cout << "✗ Error: Please provide a pattern\n";
            } else {
                std::cout << "✓ Pattern set: " << pattern << "\n";
            }
            std::cout << "> ";
            continue;
        }
        
        if (line.substr(0, 5) == "input") {
            input = trim(line.substr(5));
            std::cout << "✓ Input set: " << (input.empty() ? "(empty)" : input) << "\n";
            std::cout << "> ";
            continue;
        }
        
        if (line == "trace") {
            if (pattern.empty()) {
                std::cout << "✗ Error: Please set a pattern first (pattern <regex>)\n";
            } else {
                tracePattern(pattern, input);
            }
            std::cout << "> ";
            continue;
        }
        
        if (line == "match") {
            if (pattern.empty()) {
                std::cout << "✗ Error: Please set a pattern first (pattern <regex>)\n";
            } else {
                quickMatch(pattern, input);
            }
            std::cout << "> ";
            continue;
        }
        
        if (line == "debug") {
            if (pattern.empty()) {
                std::cout << "✗ Error: Please set a pattern first (pattern <regex>)\n";
            } else {
                debugPattern(pattern, input);
            }
            std::cout << "> ";
            continue;
        }
        
        std::cout << "✗ Unknown command: " << line << "\n";
        std::cout << "   Type 'help' for available commands\n";
        std::cout << "> ";
    }
    
    return 0;
}
