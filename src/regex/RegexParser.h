#ifndef REGEX_PARSER_H
#define REGEX_PARSER_H

#include "NFA.h"
#include "DFA.h"
#include <string>
#include <vector>

namespace Regex {

// Regular Expression Parser
// Converts regex pattern to NFA using Thompson Construction
class RegexParser {
private:
    std::string pattern;
    size_t position;
    
    char peek();
    char consume();
    bool isAtEnd();
    
    // Recursive descent parser for regex
    NFA parseExpression();
    NFA parseTerm();
    NFA parseFactor();
    NFA parseAtom();
    NFA parseCharClass();
    
    bool isMetaChar(char c);
    
public:
    RegexParser(const std::string& pattern);
    
    // Parse regex and return NFA
    NFA parse();
    
    // Parse and convert to DFA directly
    DFA parseToDFA();
};

// Regular Expression Engine
class RegexEngine {
private:
    DFA dfa;
    std::string pattern;
    
public:
    RegexEngine(const std::string& pattern);
    
    bool match(const std::string& input);
    bool search(const std::string& input);
    std::vector<std::pair<size_t, size_t>> findAll(const std::string& input);
};

} // namespace Regex

#endif // REGEX_PARSER_H
