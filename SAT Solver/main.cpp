#include <iostream>
#include "dpll.h"
#include <cmath> // For abs()
#include <chrono> //For algo runtime

// Simple Heuristic of choosing the first unassign literal following the slide example
int normHeuristic(currentState& state, const Espression& cEspression){
    for (int i = 1; i <= state.numLiterals; i++) { // Start at 1 for DIMACs consistency
        if (state.assignment[i] == UNASSIGNED) {
            return i; // Return the first unassigned literal
        }
    }
    return 0; // No unassigned literals found
}

// Reference https://www.cs.cmu.edu/~emc/15-820A/reading/sat_cmu.pdf
int dlisHeuristic(currentState& state, const Espression& cEspression){
    // Following Pseudocode: vectors holding the count for both + and - literals
    std::vector<int> posCount(state.numLiterals +1, 0);
    std::vector<int> negCount(state.numLiterals +1, 0);

    std::vector<int> tUnassigned; // Unassigned temporary vector
    tUnassigned.reserve(state.numLiterals); // Reserve memory


    // Query clauses to add to count vectors
    for (int i = 0; i< cEspression.size(); i++){
        bool isSat = false; //Initial state is false
        tUnassigned.clear(); // Clear the vector. Reuse for less memory

        for(int j = 0; j< cEspression[i].size();j++){
            int lit = cEspression[i][j];
            int varIdx= std::abs(lit); //same literal regardless true or false
            LiteralValue val = state.assignment[varIdx];

            // CHeck if sat already
            if((val == TRUEVal && lit >0) || (val == FALSEVal && lit < 0)){
                isSat = true;
                break;
            }else if (val == UNASSIGNED){
                tUnassigned.push_back(lit);
            }
        }
        if (!isSat){
            for (int k = 0; k <tUnassigned.size(); k++){
                int lit = tUnassigned[k];
                if(lit > 0){
                    posCount[std::abs(lit)]++;
                } else{
                    negCount[std::abs(lit)]++;
                }
            }
        }
    }

    int maxPosCount = -1; //Set to be beat on first iteration
    int maxNegCount = -1;

    int bestPos = 0; // x in pseudo code, index for variable
    int bestNeg = 0; // y in pseudo code, index for variable

    // If Cx,p > Cy,n choose x and assign it TRUE Otherwise choose y and assign it FALSE
    for (int i = 1; i<= state.numLiterals; i++){
        if (state.assignment[i] == UNASSIGNED){
            if (posCount[i] > maxPosCount){
                maxPosCount = posCount[i];
                bestPos = i; 
            }

            if (negCount[i] > maxNegCount){
                maxNegCount = negCount[i];
                bestNeg = i; 
            }
        }
    }

    // Edge case no literals left in clauses 
    // let the normal heurisitc choose to not get stuck
    if (bestPos == 0 && bestNeg == 0){
        return normHeuristic(state, cEspression);
    }

    if (maxPosCount > maxNegCount){
        return bestPos;
    }else {
        return bestNeg;
    }
}

int main(int argc, char* argv[]){
    if(argc < 2 || argc > 3){
        std::cout << "Incorrect arguments. Do ./SATsolver file.cnf [--dlis]" << std::endl;
        return 1;
    }

    bool useDlis = false;
    if(argc ==3){
        std::string flag = argv[2]; // Save flag as string
        if (flag == "--dlis"){
            useDlis = true;
        } else{
            std::cout << "Wrong flag. Defaulting" << std::endl;
        }
    }


    // Parse debug msg remove later
    //std::cout << "Parsing" << std::endl;
    CNF input = parse_dimacs(argv[1]); //call dimacs parser
    std::function<int(currentState&, const Espression&)> heuristicSel;

    if(useDlis){
        heuristicSel = dlisHeuristic;
    } else{
        heuristicSel = normHeuristic;
    }

    // Solver time
    SATSolver solver(&input, heuristicSel); // Populate
    //std::cout << "Populated" << std::endl;

    auto start = std::chrono::high_resolution_clock::now(); //start time
    solver.solve(); // Solve
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> total = end - start;
    std::cout << "Solve Time:" << total.count() << " seconds" <<std::endl;

    //std::cout << "Solved" << std::endl;
    solver.printAssignment(); // Print the assignment
    //std::cout << "Printed" << std::endl;
    
    free_cnf(&input); // Free the CNF memory
    //std::cout << "Freed mem" << std::endl;
};