#include <iostream>
#include "dpll.h"

// Simple Heuristic of choosing the first unassign literal following the slide example
int normHeuristic(currentState& state){
    for (int i = 1; i <= state.numLiterals; i++) { // Start at 1 for DIMACs consistency
        if (state.assignment[i] == UNASSIGNED) {
            return i; // Return the first unassigned literal
        }
    }
    return 0; // No unassigned literals found
}

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cout << "Incorrect arguments. Do ./SATsolver file.cnf" << std::endl;
        return 1;
    }

    // Parse debug msg remove later
    //std::cout << "Parsing" << std::endl;
    CNF input = parse_dimacs(argv[1]); //call dimacs parser

    // Solver time
    SATSolver solver(&input, normHeuristic); // Populate
    //std::cout << "Populated" << std::endl;
    solver.solve(); // Solve
    //std::cout << "Solved" << std::endl;
    solver.printAssignment(); // Print the assignment
    //std::cout << "Printed" << std::endl;
    
    free_cnf(&input); // Free the CNF memory
    //std::cout << "Freed mem" << std::endl;
};