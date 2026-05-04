#include "dpll.h"
#include <algorithm>
#include <cmath>

//Constructor
SATSolver::SATSolver(const CNF* parCNF, std::function<int(currentState&, const Espression&)> heuristicFunc) : heuristic(heuristicFunc) {
    // Initialize the current state with the parsed CNF
    state.numLiterals = parCNF->num_vars;

    // Init literals as unassigned and +1 indexing for DIMACs consistency
    state.assignment.resize(state.numLiterals + 1, UNASSIGNED); // +1

    // Convert CNF clauses
    for (int i = 0; i < parCNF->num_clauses; i++) {
        Term term; // Create a term (C++) to hold the C arrays
        for (int j = 0; j < parCNF->clause_sizes[i]; j++) {
            term.push_back(parCNF->clauses[i][j]); // Add literals one by one
        }
        this->cEspression.push_back(term); // Add the full term to expression
    }
}

// BCP Func based on Unit 5 slide 108 pseudocode 
void SATSolver::BCP(currentState& state) {
    state.Sat = true; // Assume satisfied 

    while(true){
        int unitLiteral = 0; // Init unitLiteral tracker

        // For each clause
        // can start at 0 since we are not using assignment array
        for (int i = 0; i < this->cEspression.size(); i++){
            int count = 0; // Literal count "Count =0"
            bool localSat = false; // Local satisfaction tracker
            int unassignedLit = 0; // Unassigned literal tracker

            // For each literal in clause
            for(int j = 0; j < this->cEspression[i].size(); j++){
                int lit = this->cEspression[i][j]; // Get literal
                int varIdx = std::abs(lit); // Get variable index (abs value of literal)
                LiteralValue val = state.assignment[varIdx]; //Get val for var
                
                // If(l==X) count++
                if (val == UNASSIGNED){
                    count++;
                    unassignedLit = lit; // Track unassigned literal
                
                // If (l==1) Not unit clause
                } else if ((val == TRUEVal && lit > 0) || (val == FALSEVal && lit < 0)){
                    localSat = true; // Clause sat
                    break; // break out of literal loop
                }
            }

            // If clause not satisfied and count == 0, conflict
            if (!localSat && count == 0){
                state.Sat = false; // Mark unsatisfied
                return; // Exit BCP
            }
            // If(count == 1) unit clause
            if (!localSat && count == 1){
                unitLiteral = unassignedLit; // Track unit literal
                break; // Break out of clause loop for forced move
            }
            // Else not unit clause
        }

        // If no unit literal, break out of while loop
        if (unitLiteral == 0){
            break;
        }

        // Apply forced move and restart loop
        int varIndex = std::abs(unitLiteral); // Get variable index

        if (unitLiteral > 0){
            state.assignment[varIndex] = TRUEVal;
        } else {
            state.assignment[varIndex] = FALSEVal;
        }
    }
}

// Followed PureLiteral Elimination Pseudo code from brown
//https://cs.brown.edu/courses/cs195y/2016/assignments/sat-lab.html
void SATSolver::PureLiteralElimination(currentState& state){
    
    // loop till no pure literals
    while(true){
        int pureLiteralCount = 0; // tracker for exit
        
        // Due to in-place updating to limit memory usage, we check for pure literals
        
        // arrays to track +/- of literals
        std::vector<int> posXArr(state.numLiterals + 1, 0); // +1 for DIMACs consistency
        std::vector<int> negXArr(state.numLiterals + 1, 0);


        // For each clause, check for pure literals and add to arrays
        for (int i = 0; i < this->cEspression.size(); i++){
            bool isSat = false; //Local sat tracker since in-place and may already be satisfied
            
            for(int j = 0; j < this->cEspression[i].size(); j++){
                int lit = this->cEspression[i][j]; // Get literal
                int varIdx = std::abs(lit); // Get var index
                LiteralValue val = state.assignment[varIdx]; // Get val for var
                if ((val == TRUEVal && lit > 0) || (val == FALSEVal && lit < 0)){
                    isSat = true; // Clause sat, skip clause
                    break;
                }
            }

            if(isSat){
                continue; // skip clause if sat
            }

            // If not satisfied, check for pure literals
            // 2nd for loop to check for pure lit in unsat clauses
            // This is due to in-place updates
            for(int k = 0; k < this->cEspression[i].size(); k++){
                int lit = this->cEspression[i][k]; // Get literal
                int varIdx = std::abs(lit); // Get var index
                LiteralValue val = state.assignment[varIdx]; // Get val for var

                if (val == UNASSIGNED){ // Only consider unassigned literals for purity
                    if (lit > 0){
                        posXArr[varIdx] = 1;
                    } else {
                        negXArr[varIdx] = 1;
                    }
                }
            }
        }
        //for each variable x
        for(int l = 1; l <= state.numLiterals; l++){
            // check if literal is unassigned
            if(state.assignment[l]==UNASSIGNED){
                // if +x is pure in F
                if(posXArr[l] == 1 && negXArr[l] == 0){ // If pos literal is pure
                    
                    state.assignment[l] = TRUEVal; // Set literal to true is equiv to remove all clauses containing +x
                    // add a unit clause{+x} is unneccessary due to in-place update
                    
                    pureLiteralCount++; // Increment pure literal count
                
                // if -x is pure in F
                } else if (negXArr[l] == 1 && posXArr[l] == 0){ // If neg literal is pure
                    state.assignment[l] = FALSEVal; // Set literal to false
                    pureLiteralCount++; // Increment pure literal count
                }
            }
        }
        if(pureLiteralCount == 0){
            break; // If no pure literals, break out of while loop
        }
    }
}


// Followed DPLL pseudo code from wikipedia
// https://en.wikipedia.org/wiki/DPLL_algorithm
// Followed Recursive Backtracking pseudo code from Brown
// https://cs.brown.edu/courses/cs195y/2016/assignments/sat-lab.html
bool SATSolver::DPLL(currentState state){
    
    // while there is a unit clause {l} in Φ do
    //    Φ <- BCP(l, Φ)
    BCP(state);

    // pure Literal elimination
    //while there is a literal l that occurs pure in Φ do
    //    Φ <- PureLiteralElimination(l, Φ)
    PureLiteralElimination(state);

    // Stopping conditions
    // if Φ is empty then return true
    bool phiEmpty = true; //assume empty

    // Start at 1 for DIMACs consistency
    for(int i = 1; i <= state.numLiterals; i++){
        if (state.assignment[i] == UNASSIGNED){
            phiEmpty = false; // empty if any literal is unassigned
            break;
        }
    }

    // double check
    if(phiEmpty && state.Sat){
        this->state = state; // save winning state assign use this-> for in-place update
        return true;
    }

    // if Φ contains an empty clause then return false
    if (!state.Sat){
        return false;
    }
    
    //DPLL Procedure:
    // l <- choose-literal(Φ) | Heuristic func
    int chosenLiteral = heuristic(state, this->cEspression);
    //op

    // return DPLL(Φ ∧ l) or DPLL(Φ ∧ ¬l)

    // try adding the literal to the formula w/ and run dpll
    currentState newTState = state; // Create  new state for dpll call
    newTState.assignment[chosenLiteral] = TRUEVal; // Add literal to new state

    if (DPLL(newTState)){
        return true; // If sat, return true
    }

    // Try when literal is false
    currentState newFState = state; // Create  new state for dpll call
    newFState.assignment[chosenLiteral] = FALSEVal; // Add neg Literal

    return DPLL(newFState); // Return result of dpll call
}

void SATSolver::solve(){
    state.Sat = DPLL(state); // Start DPLL
}

void SATSolver::printAssignment(){
    if(state.Sat == true){
        std::cout << "RESULT:SAT" << std::endl << "ASSIGNMENT:";
        for (int i = 1; i <= state.numLiterals; i++){
            if(state.assignment[i] == TRUEVal){
                std::cout << i << "=1" << " ";
            } else if (state.assignment[i] == FALSEVal){
                std::cout << i << "=0" << " ";
            }
        }
        std::cout << std::endl;
    }else{
        std::cout << "RESULT:UNSAT" << std::endl;  
    }  
}