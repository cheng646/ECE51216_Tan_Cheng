#ifndef dpll_h
#define dpll_h

#include <vector>
#include <iostream>
#include <functional>

// C linkage for the Dimacs parser
extern "C" {
    // Dimacs parser struct
    typedef struct {
        int num_vars;
        int num_clauses;
        int** clauses;
        int* clause_sizes;
    } CNF;

    // Dimacs parser function prototypes
    CNF parse_dimacs(const char* filename);
    void free_cnf(CNF* cnf);
}


// A term (ab) is a vector of literals
typedef std::vector<int> Term;
// An espression is a vector of terms (ab + bc)
typedef std::vector<Term> Espression;

// Definitions for literals
typedef enum{
    FALSEVal = -1, // False is -1
    TRUEVal = 1, // True is 1
    UNASSIGNED = 0 // Unassigned is 0
} LiteralValue;

// Current state of Solver
typedef struct {
    Espression cEspression; // Current espression of clauses and literals
    std::vector<LiteralValue> assignment; //Assignment for sat/unsat mapped to literals
    int numLiterals; // Number of literals in the problem
    bool Sat; // Satisfied? For BCP and DPLL to update
} currentState;

class SATSolver {
private:
    // Holding espression and assignment (For full espression)
    currentState state;

    void BCP(currentState& state); // Binary Constraint Prop Func, return false for conflicts
    void PureLiteralElimination(currentState& state); // From DPLL pseudo code
    std::function<int(currentState&)> heuristic; // Heuristic pointer function for variable selection (can choose DLIS in main)
public:
    //Constructor which parses the CNF file and initializes relevant structs, takes heuristic function arg
    SATSolver(const CNF* parCNF, std::function<int(currentState&)> heuristicFunc);

    //DPLL Recursive Function
    bool DPLL(currentState state);

    // Helper funcs for encapsulation
    void solve();
    void printAssignment();
};

#endif /* dpll_h */