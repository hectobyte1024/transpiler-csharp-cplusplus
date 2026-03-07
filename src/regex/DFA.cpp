#include "DFA.h"
#include "NFA.h"
#include <queue>
#include <algorithm>

namespace Regex {

DFA::DFA() : nextStateId(0), startState(nullptr) {}

DFAState* DFA::createState(bool accepting) {
    states.push_back(std::make_unique<DFAState>(nextStateId++, accepting));
    DFAState* newState = states.back().get();
    if (accepting) {
        acceptStates.insert(newState);
    }
    return newState;
}

// Subset Construction Algorithm: Convert NFA to DFA
DFA DFA::fromNFA(const NFA& nfa) {
    DFA dfa;
    
    // Map from set of NFA states to DFA state
    std::map<std::set<NFAState*>, DFAState*> stateMap;
    std::queue<std::set<NFAState*>> unmarkedStates;
    
    // Get alphabet (all distinct characters in transitions)
    std::set<char> alphabet;
    for (const auto& state : nfa.getStates()) {
        for (const auto& trans : state->transitions) {
            alphabet.insert(trans.first);
        }
    }
    
    // Start with epsilon closure of NFA start state
    std::set<NFAState*> startSet;
    startSet.insert(nfa.getStartState());
    std::set<NFAState*> startClosure;
    
    // Manual epsilon closure for start state
    std::queue<NFAState*> epsQueue;
    epsQueue.push(nfa.getStartState());
    startClosure.insert(nfa.getStartState());
    
    while (!epsQueue.empty()) {
        NFAState* current = epsQueue.front();
        epsQueue.pop();
        
        for (NFAState* next : current->epsilonTransitions) {
            if (startClosure.find(next) == startClosure.end()) {
                startClosure.insert(next);
                epsQueue.push(next);
            }
        }
    }
    
    // Check if start state should be accepting
    bool startAccepting = false;
    for (NFAState* nfaState : startClosure) {
        if (nfa.getAcceptStates().find(nfaState) != nfa.getAcceptStates().end()) {
            startAccepting = true;
            break;
        }
    }
    
    DFAState* dfaStart = dfa.createState(startAccepting);
    dfaStart->nfaStates = startClosure;
    dfa.setStartState(dfaStart);
    
    stateMap[startClosure] = dfaStart;
    unmarkedStates.push(startClosure);
    
    // Process unmarked states
    while (!unmarkedStates.empty()) {
        std::set<NFAState*> currentNFAStates = unmarkedStates.front();
        unmarkedStates.pop();
        
        DFAState* currentDFAState = stateMap[currentNFAStates];
        
        // For each symbol in alphabet
        for (char symbol : alphabet) {
            std::set<NFAState*> nextNFAStates;
            
            // Move on symbol
            for (NFAState* nfaState : currentNFAStates) {
                auto it = nfaState->transitions.find(symbol);
                if (it != nfaState->transitions.end()) {
                    for (NFAState* next : it->second) {
                        nextNFAStates.insert(next);
                    }
                }
            }
            
            if (nextNFAStates.empty()) {
                continue;
            }
            
            // Epsilon closure of next states
            std::set<NFAState*> nextClosure;
            std::queue<NFAState*> closureQueue;
            
            for (NFAState* state : nextNFAStates) {
                closureQueue.push(state);
                nextClosure.insert(state);
            }
            
            while (!closureQueue.empty()) {
                NFAState* current = closureQueue.front();
                closureQueue.pop();
                
                for (NFAState* next : current->epsilonTransitions) {
                    if (nextClosure.find(next) == nextClosure.end()) {
                        nextClosure.insert(next);
                        closureQueue.push(next);
                    }
                }
            }
            
            // Check if this DFA state already exists
            DFAState* nextDFAState;
            if (stateMap.find(nextClosure) == stateMap.end()) {
                // Create new DFA state
                bool isAccepting = false;
                for (NFAState* nfaState : nextClosure) {
                    if (nfa.getAcceptStates().find(nfaState) != nfa.getAcceptStates().end()) {
                        isAccepting = true;
                        break;
                    }
                }
                
                nextDFAState = dfa.createState(isAccepting);
                nextDFAState->nfaStates = nextClosure;
                stateMap[nextClosure] = nextDFAState;
                unmarkedStates.push(nextClosure);
            } else {
                nextDFAState = stateMap[nextClosure];
            }
            
            // Add transition
            currentDFAState->transitions[symbol] = nextDFAState;
        }
    }
    
    return dfa;
}

// DFA Minimization (Hopcroft's algorithm - simplified)
void DFA::minimize() {
    // This is a simplified version
    // Full implementation would use partition refinement
    // For now, we skip minimization as it's an optimization
}

// Simulate DFA on input string
bool DFA::simulate(const std::string& input) {
    if (!startState) {
        return false;
    }
    
    DFAState* currentState = startState;
    
    for (char c : input) {
        auto it = currentState->transitions.find(c);
        if (it == currentState->transitions.end()) {
            return false; // No transition for this symbol
        }
        currentState = it->second;
    }
    
    return currentState->isAccepting;
}

} // namespace Regex
