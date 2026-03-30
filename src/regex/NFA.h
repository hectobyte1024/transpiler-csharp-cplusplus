#ifndef NFA_H
#define NFA_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>

namespace Regex {

// NFA State
struct NFAState {
    int id;
    bool isAccepting;
    std::map<char, std::vector<NFAState*>> transitions;
    std::vector<NFAState*> epsilonTransitions;
    
    NFAState(int id, bool accepting = false) 
        : id(id), isAccepting(accepting) {}
};

// NFA Fragment for Thompson Construction
struct NFAFragment {
    NFAState* start;
    NFAState* end;
    
    NFAFragment(NFAState* s, NFAState* e) : start(s), end(e) {}
};

// Non-deterministic Finite Automaton
class NFA {
private:
    std::vector<std::unique_ptr<NFAState>> states;
    int nextStateId;
    NFAState* startState;
    std::set<NFAState*> acceptStates;
    
    NFAState* createState(bool accepting = false);
    std::set<NFAState*> epsilonClosure(const std::set<NFAState*>& states);
    
public:
    NFA();
    ~NFA() = default;
    
    // Disable copy, enable move
    NFA(const NFA&) = delete;
    NFA& operator=(const NFA&) = delete;
    NFA(NFA&&) = default;
    NFA& operator=(NFA&&) = default;
    
    // Thompson Construction operations
    static NFA epsilon();  // Matches empty string
    static NFA fromChar(char c);
    static NFA concatenate(NFA&& left, NFA&& right);
    static NFA alternate(NFA&& left, NFA&& right);
    static NFA kleeneStar(NFA&& nfa);
    static NFA plus(NFA&& nfa);
    static NFA optional(NFA&& nfa);
    
    NFAState* getStartState() const { return startState; }
    const std::set<NFAState*>& getAcceptStates() const { return acceptStates; }
    const std::vector<std::unique_ptr<NFAState>>& getStates() const { return states; }
    
    std::set<NFAState*> epsilonClosure(NFAState* state);
    std::set<NFAState*> move(const std::set<NFAState*>& states, char symbol);
    
    bool simulate(const std::string& input);
};

} // namespace Regex

#endif // NFA_H
