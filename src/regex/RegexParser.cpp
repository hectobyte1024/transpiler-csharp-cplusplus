#include "RegexParser.h"
#include <stdexcept>
#include <cctype>

namespace Regex {

RegexParser::RegexParser(const std::string& pattern) 
    : pattern(pattern), position(0) {}

char RegexParser::peek() {
    if (isAtEnd()) return '\0';
    return pattern[position];
}

char RegexParser::consume() {
    if (isAtEnd()) return '\0';
    return pattern[position++];
}

bool RegexParser::isAtEnd() {
    return position >= pattern.length();
}

bool RegexParser::isMetaChar(char c) {
    return c == '|' || c == '*' || c == '+' || c == '?' || 
           c == '(' || c == ')' || c == '[' || c == ']' ||
           c == '\\' || c == '.';
}

// Expression -> Term ('|' Term)*
NFA RegexParser::parseExpression() {
    NFA result = parseTerm();
    
    while (peek() == '|') {
        consume(); // consume '|'
        NFA right = parseTerm();
        result = NFA::alternate(std::move(result), std::move(right));
    }
    
    return result;
}

// Term -> Factor Factor*
NFA RegexParser::parseTerm() {
    NFA result = parseFactor();
    
    while (!isAtEnd() && peek() != ')' && peek() != '|') {
        NFA right = parseFactor();
        result = NFA::concatenate(std::move(result), std::move(right));
    }
    
    return result;
}

// Factor -> Atom ('*' | '+' | '?')?
NFA RegexParser::parseFactor() {
    NFA result = parseAtom();
    
    if (!isAtEnd()) {
        char c = peek();
        if (c == '*') {
            consume();
            result = NFA::kleeneStar(std::move(result));
        } else if (c == '+') {
            consume();
            result = NFA::plus(std::move(result));
        } else if (c == '?') {
            consume();
            result = NFA::optional(std::move(result));
        }
    }
    
    return result;
}

// Atom -> char | '.' | '(' Expression ')' | '[' CharClass ']'
NFA RegexParser::parseAtom() {
    if (isAtEnd()) {
        throw std::runtime_error("Unexpected end of pattern");
    }
    
    char c = peek();
    
    // Parenthesized expression
    if (c == '(') {
        consume(); // consume '('
        NFA result = parseExpression();
        if (peek() != ')') {
            throw std::runtime_error("Expected ')'");
        }
        consume(); // consume ')'
        return result;
    }
    
    // Character class
    if (c == '[') {
        return parseCharClass();
    }
    
    // Wildcard
    if (c == '.') {
        consume();
        // Create alternation for printable ASCII
        NFA result = NFA::fromChar(' ');
        for (char ch = '!'; ch <= '~'; ch++) {
            result = NFA::alternate(std::move(result), NFA::fromChar(ch));
        }
        return result;
    }
    
    // Escape sequence
    if (c == '\\') {
        consume();
        if (isAtEnd()) {
            throw std::runtime_error("Unexpected end after '\\'");
        }
        c = consume();
        
        // Handle special escape sequences
        switch (c) {
            case 'n': return NFA::fromChar('\n');
            case 't': return NFA::fromChar('\t');
            case 'r': return NFA::fromChar('\r');
            case 'd': { // digit
                NFA result = NFA::fromChar('0');
                for (char ch = '1'; ch <= '9'; ch++) {
                    result = NFA::alternate(std::move(result), NFA::fromChar(ch));
                }
                return result;
            }
            case 'w': { // word character [a-zA-Z0-9_]
                NFA result = NFA::fromChar('_');
                for (char ch = 'a'; ch <= 'z'; ch++) {
                    result = NFA::alternate(std::move(result), NFA::fromChar(ch));
                }
                for (char ch = 'A'; ch <= 'Z'; ch++) {
                    result = NFA::alternate(std::move(result), NFA::fromChar(ch));
                }
                for (char ch = '0'; ch <= '9'; ch++) {
                    result = NFA::alternate(std::move(result), NFA::fromChar(ch));
                }
                return result;
            }
            case 's': { // whitespace
                NFA result = NFA::fromChar(' ');
                result = NFA::alternate(std::move(result), NFA::fromChar('\t'));
                result = NFA::alternate(std::move(result), NFA::fromChar('\n'));
                result = NFA::alternate(std::move(result), NFA::fromChar('\r'));
                return result;
            }
            default:
                return NFA::fromChar(c); // Escaped meta character
        }
    }
    
    // Regular character
    consume();
    return NFA::fromChar(c);
}

// CharClass -> '[' char-char ']'
NFA RegexParser::parseCharClass() {
    consume(); // consume '['
    
    bool negate = false;
    if (peek() == '^') {
        negate = true;
        consume();
    }
    
    std::vector<char> chars;
    
    while (!isAtEnd() && peek() != ']') {
        char start = consume();
        
        // Range
        if (peek() == '-' && position + 1 < pattern.length() && pattern[position + 1] != ']') {
            consume(); // consume '-'
            char end = consume();
            
            for (char c = start; c <= end; c++) {
                chars.push_back(c);
            }
        } else {
            chars.push_back(start);
        }
    }
    
    if (peek() != ']') {
        throw std::runtime_error("Expected ']'");
    }
    consume(); // consume ']'
    
    if (chars.empty()) {
        throw std::runtime_error("Empty character class");
    }
    
    // Create alternation of all characters
    NFA result = NFA::fromChar(chars[0]);
    for (size_t i = 1; i < chars.size(); i++) {
        result = NFA::alternate(std::move(result), NFA::fromChar(chars[i]));
    }
    
    // TODO: Handle negation properly
    // For now, we ignore negation as it requires full alphabet knowledge
    
    return result;
}

NFA RegexParser::parse() {
    return parseExpression();
}

DFA RegexParser::parseToDFA() {
    NFA nfa = parse();
    return DFA::fromNFA(nfa);
}

// RegexEngine implementation
RegexEngine::RegexEngine(const std::string& pattern) : pattern(pattern) {
    RegexParser parser(pattern);
    dfa = parser.parseToDFA();
}

bool RegexEngine::match(const std::string& input) {
    return dfa.simulate(input);
}

bool RegexEngine::search(const std::string& input) {
    // Try to match at each position
    for (size_t i = 0; i < input.length(); i++) {
        for (size_t j = i; j <= input.length(); j++) {
            std::string substr = input.substr(i, j - i);
            if (dfa.simulate(substr)) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::pair<size_t, size_t>> RegexEngine::findAll(const std::string& input) {
    std::vector<std::pair<size_t, size_t>> matches;
    
    for (size_t i = 0; i < input.length(); i++) {
        for (size_t j = i + 1; j <= input.length(); j++) {
            std::string substr = input.substr(i, j - i);
            if (dfa.simulate(substr)) {
                matches.push_back({i, j});
                break; // Take longest match from this position
            }
        }
    }
    
    return matches;
}

} // namespace Regex
