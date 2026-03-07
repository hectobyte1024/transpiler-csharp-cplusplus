#ifndef DFA_H
#define DFA_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>

namespace Regex {

// Forward declaration
class NFA;
struct NFAState;

// DFA State
struct DFAState {
    int id;
    bool isAccepting;
    std::map<char, DFAState*> transitions;
    std::set<NFAState*> nfaStates; // For subset construction
    
    DFAState(int id, bool accepting = false) 
        : id(id), isAccepting(accepting) {}
};

// Deterministic Finite Automaton
class DFA {
private:
    std::vector<std::unique_ptr<DFAState>> states;
    int nextStateId;
    DFAState* startState;
    std::set<DFAState*> acceptStates;
    
    DFAState* createState(bool accepting = false);
    
public:
    DFA();
    ~DFA() = default;
    
    // Disable copy, enable move
    DFA(const DFA&) = delete;
    DFA& operator=(const DFA&) = delete;
    DFA(DFA&&) = default;
    DFA& operator=(DFA&&) = default;
    
    // Subset Construction: Convert NFA to DFA
    static DFA fromNFA(const NFA& nfa);
    
    // DFA Minimization (optional optimization)
    void minimize();
    
    // Simulate DFA on input
    bool simulate(const std::string& input);
    
    DFAState* getStartState() const { return startState; }
    const std::set<DFAState*>& getAcceptStates() const { return acceptStates; }
    const std::vector<std::unique_ptr<DFAState>>& getStates() const { return states; }
    
    void setStartState(DFAState* state) { startState = state; }
    void addAcceptState(DFAState* state) { acceptStates.insert(state); }
};

} // namespace Regex

#endif // DFA_H
