#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int num_vars;
    int num_clauses;
    int** clauses;
    int* clause_sizes;
} CNF;

// ---------------- PARSER ----------------
CNF parse_dimacs(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Error opening file");
        exit(1);
    }

    CNF cnf;
    cnf.num_vars = 0;
    cnf.num_clauses = 0;
    cnf.clauses = NULL;
    cnf.clause_sizes = NULL;

    char line[1024];
    int clause_idx = 0;

    while (fgets(line, sizeof(line), f)) {
        // Skip comments and empty lines
        if (line[0] == 'c' || line[0] == '\n')
            continue;

        // Problem line
        if (line[0] == 'p') {
            if (sscanf(line, "p cnf %d %d", &cnf.num_vars, &cnf.num_clauses) != 2) {
                fprintf(stderr, "Invalid DIMACS header\n");
                exit(1);
            }

            // Allocate memory
            cnf.clauses = malloc(cnf.num_clauses * sizeof(int*));
            cnf.clause_sizes = malloc(cnf.num_clauses * sizeof(int));
            continue;
        }

        // Parse clause
        int capacity = 8;
        int size = 0;
        int* clause = malloc(capacity * sizeof(int));

        char* ptr = line;
        int lit;

        while (sscanf(ptr, "%d", &lit) == 1) {
            // Move pointer forward
            while (*ptr && *ptr != ' ') ptr++;
            while (*ptr == ' ') ptr++;

            if (lit == 0)
                break;

            if (size >= capacity) {
                capacity *= 2;
                clause = realloc(clause, capacity * sizeof(int));
            }

            clause[size++] = lit;
        }

        if (size > 0) {
            cnf.clauses[clause_idx] = clause;
            cnf.clause_sizes[clause_idx] = size;
            clause_idx++;
        } else {
            free(clause);
        }
    }

    fclose(f);
    return cnf;
}

// ---------------- CLEANUP ----------------
void free_cnf(CNF* cnf) {
    for (int i = 0; i < cnf->num_clauses; i++) {
        free(cnf->clauses[i]);
    }
    free(cnf->clauses);
    free(cnf->clause_sizes);
}