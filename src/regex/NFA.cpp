#include "NFA.h"
#include <queue>
#include <algorithm>

namespace Regex {

NFA::NFA() : nextStateId(0), startState(nullptr) {}

NFAState* NFA::createState(bool accepting) {
    states.push_back(std::make_unique<NFAState>(nextStateId++, accepting));
    NFAState* newState = states.back().get();
    if (accepting) {
        acceptStates.insert(newState);
    }
    return newState;
}

// Thompson Construction: Create NFA for single character
NFA NFA::fromChar(char c) {
    NFA nfa;
    NFAState* start = nfa.createState(false);
    NFAState* end = nfa.createState(true);
    
    start->transitions[c].push_back(end);
    nfa.startState = start;
    
    return nfa;
}

// Thompson Construction: Concatenation
NFA NFA::concatenate(NFA&& left, NFA&& right) {
    NFA result;
    
    // Transfer left states
    for (auto& state : left.states) {
        result.states.push_back(std::move(state));
    }
    
    // Transfer right states
    for (auto& state : right.states) {
        result.states.push_back(std::move(state));
    }
    
    // Connect left accepting states to right start with epsilon
    for (auto* acceptState : left.acceptStates) {
        acceptState->isAccepting = false;
        acceptState->epsilonTransitions.push_back(right.startState);
    }
    
    result.startState = left.startState;
    result.acceptStates = right.acceptStates;
    result.nextStateId = left.nextStateId + right.nextStateId;
    
    return result;
}

// Thompson Construction: Alternation (|)
NFA NFA::alternate(NFA&& left, NFA&& right) {
    NFA result;
    
    NFAState* newStart = result.createState(false);
    NFAState* newEnd = result.createState(true);
    
    // Transfer left and right states
    for (auto& state : left.states) {
        result.states.push_back(std::move(state));
    }
    for (auto& state : right.states) {
        result.states.push_back(std::move(state));
    }
    
    // New start has epsilon transitions to both old starts
    newStart->epsilonTransitions.push_back(left.startState);
    newStart->epsilonTransitions.push_back(right.startState);
    
    // Both old accept states transition to new accept state
    for (auto* acceptState : left.acceptStates) {
        acceptState->isAccepting = false;
        acceptState->epsilonTransitions.push_back(newEnd);
    }
    for (auto* acceptState : right.acceptStates) {
        acceptState->isAccepting = false;
        acceptState->epsilonTransitions.push_back(newEnd);
    }
    
    result.startState = newStart;
    result.acceptStates.clear();
    result.acceptStates.insert(newEnd);
    
    return result;
}

// Thompson Construction: Kleene Star (*)
NFA NFA::kleeneStar(NFA&& nfa) {
    NFA result;
    
    NFAState* newStart = result.createState(false);
    NFAState* newEnd = result.createState(true);
    
    // Transfer nfa states
    for (auto& state : nfa.states) {
        result.states.push_back(std::move(state));
    }
    
    // New start to old start and new end (epsilon)
    newStart->epsilonTransitions.push_back(nfa.startState);
    newStart->epsilonTransitions.push_back(newEnd);
    
    // Old accept states to old start (loop back) and new end
    for (auto* acceptState : nfa.acceptStates) {
        acceptState->isAccepting = false;
        acceptState->epsilonTransitions.push_back(nfa.startState);
        acceptState->epsilonTransitions.push_back(newEnd);
    }
    
    result.startState = newStart;
    result.acceptStates.clear();
    result.acceptStates.insert(newEnd);
    
    return result;
}

// Thompson Construction: Plus (+)
NFA NFA::plus(NFA&& nfa) {
    NFA result;
    
    NFAState* newEnd = result.createState(true);
    
    // Transfer nfa states
    for (auto& state : nfa.states) {
        result.states.push_back(std::move(state));
    }
    
    // Old accept states to old start (loop back) and new end
    for (auto* acceptState : nfa.acceptStates) {
        acceptState->isAccepting = false;
        acceptState->epsilonTransitions.push_back(nfa.startState);
        acceptState->epsilonTransitions.push_back(newEnd);
    }
    
    result.startState = nfa.startState;
    result.acceptStates.clear();
    result.acceptStates.insert(newEnd);
    
    return result;
}

// Thompson Construction: Optional (?)
NFA NFA::optional(NFA&& nfa) {
    NFA result;
    
    NFAState* newStart = result.createState(false);
    NFAState* newEnd = result.createState(true);
    
    // Transfer nfa states
    for (auto& state : nfa.states) {
        result.states.push_back(std::move(state));
    }
    
    // New start to old start and new end (epsilon)
    newStart->epsilonTransitions.push_back(nfa.startState);
    newStart->epsilonTransitions.push_back(newEnd);
    
    // Old accept states to new end
    for (auto* acceptState : nfa.acceptStates) {
        acceptState->isAccepting = false;
        acceptState->epsilonTransitions.push_back(newEnd);
    }
    
    result.startState = newStart;
    result.acceptStates.clear();
    result.acceptStates.insert(newEnd);
    
    return result;
}

// Epsilon closure of a single state
std::set<NFAState*> NFA::epsilonClosure(NFAState* state) {
    std::set<NFAState*> closure;
    std::queue<NFAState*> queue;
    
    queue.push(state);
    closure.insert(state);
    
    while (!queue.empty()) {
        NFAState* current = queue.front();
        queue.pop();
        
        for (NFAState* nextState : current->epsilonTransitions) {
            if (closure.find(nextState) == closure.end()) {
                closure.insert(nextState);
                queue.push(nextState);
            }
        }
    }
    
    return closure;
}

// Epsilon closure of a set of states
std::set<NFAState*> NFA::epsilonClosure(const std::set<NFAState*>& states) {
    std::set<NFAState*> closure;
    
    for (NFAState* state : states) {
        std::set<NFAState*> stateClosure = epsilonClosure(state);
        closure.insert(stateClosure.begin(), stateClosure.end());
    }
    
    return closure;
}

// Move function: given a set of states and a symbol, return reachable states
std::set<NFAState*> NFA::move(const std::set<NFAState*>& states, char symbol) {
    std::set<NFAState*> result;
    
    for (NFAState* state : states) {
        auto it = state->transitions.find(symbol);
        if (it != state->transitions.end()) {
            for (NFAState* nextState : it->second) {
                result.insert(nextState);
            }
        }
    }
    
    return result;
}

// Simulate NFA on input string
bool NFA::simulate(const std::string& input) {
    std::set<NFAState*> currentStates = epsilonClosure(startState);
    
    for (char c : input) {
        currentStates = move(currentStates, c);
        currentStates = epsilonClosure(currentStates);
        
        if (currentStates.empty()) {
            return false;
        }
    }
    
    // Check if any current state is an accepting state
    for (NFAState* state : currentStates) {
        if (acceptStates.find(state) != acceptStates.end()) {
            return true;
        }
    }
    
    return false;
}

} // namespace Regex
