#ifndef dpll_h
#define dpll_h

#include <vector>
#include <iostream>

// Dimacs parser struct
typedef struct {
    int num_vars;
    int num_clauses;
    int** clauses;
    int* clause_sizes;
} CNF;

// A term (ab) is a vector of literals
typedef std::vector<int> Term;
// An espression is a vector of terms (ab + bc)
typedef std::vector<Term> Espression;

// Definitions for literals
typedef enum{
    FALSE = 0,
    TRUE = 1,
    UNASSIGNED = -1
} LiteralValue;

// Current state of Solver
typedef struct {
    Espression cEspression; // Current espression of clauses and literals
    std::vector<LiteralValue> assignment; //Assignment for sat/unsat mapped to literals
} currentState;

class SATSolver {
private:
    // Holding espression and assignment (For full espression)
    currentState state;
    bool BCP(currentState& state);

public:

    //Constructor which parses the CNF file and initializes the data structures
    SATSolver(const CNF* parCNF);

    //DPLL Recursive Function
    bool DPLL(currentState& state);
    
    // Start the DPLL algo
    void solve();
    void printResult();
};

#endif /* dpll_h */