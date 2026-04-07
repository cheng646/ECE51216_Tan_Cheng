// ---------- DATA STRUCTURES ----------

typedef struct {
    int* lits;
    int size;
    int watch1;
    int watch2;
} Clause;

typedef struct {
    int num_vars;
    int num_clauses;

    Clause* clauses;
    int* assignment; // 1 = true, -1 = false, 0 = unassigned

    int** watch_list;
    int* watch_sizes;
    int* watch_cap;
} Solver;


// ---------- LITERAL INDEX ----------
// maps: x -> 2x, -x -> 2x+1
int lit_index(int lit) {
    return (lit > 0) ? 2 * lit : 2 * (-lit) + 1;
}


// ---------- INITIALIZE WATCH LISTS ----------

void init_watches(Solver* s) {
    int max_lit = 2 * (s->num_vars + 2);

    s->watch_list = malloc(sizeof(int*) * max_lit);
    s->watch_sizes = calloc(max_lit, sizeof(int));
    s->watch_cap = calloc(max_lit, sizeof(int));

    for (int i = 0; i < max_lit; i++) {
        s->watch_list[i] = NULL;
    }

    for (int i = 0; i < s->num_clauses; i++) {
        Clause* c = &s->clauses[i];

        c->watch1 = 0;
        c->watch2 = (c->size > 1) ? 1 : 0;

        int l1 = c->lits[c->watch1];
        int l2 = c->lits[c->watch2];

        int idx1 = lit_index(l1);
        int idx2 = lit_index(l2);

        // add watch helper
        #define ADD_WATCH(idx, cid) \
            if (s->watch_sizes[idx] >= s->watch_cap[idx]) { \
                s->watch_cap[idx] = s->watch_cap[idx] ? s->watch_cap[idx] * 2 : 4; \
                s->watch_list[idx] = realloc(s->watch_list[idx], sizeof(int) * s->watch_cap[idx]); \
            } \
            s->watch_list[idx][s->watch_sizes[idx]++] = cid;

        ADD_WATCH(idx1, i);
        ADD_WATCH(idx2, i);
    }
}


// ---------- PROPAGATION (CORE BCP) ----------

int propagate(Solver* s, int* queue, int* qhead, int* qtail) {

    while (*qhead < *qtail) {
        int lit = queue[(*qhead)++];
        int false_lit = -lit;

        int idx = lit_index(false_lit);

        int i = 0;
        while (i < s->watch_sizes[idx]) {

            int cid = s->watch_list[idx][i];
            Clause* c = &s->clauses[cid];

            int w1 = c->watch1;
            int w2 = c->watch2;

            int watch_idx = (c->lits[w1] == false_lit) ? w1 : w2;
            int other_idx = (watch_idx == w1) ? w2 : w1;

            int moved = 0;

            // try to find new literal to watch
            for (int k = 0; k < c->size; k++) {
                if (k == w1 || k == w2) continue;

                int l = c->lits[k];
                int var = abs(l);
                int val = s->assignment[var];

                if (val == 0 ||
                   (val == 1 && l > 0) ||
                   (val == -1 && l < 0)) {

                    // move watch
                    if (watch_idx == w1) c->watch1 = k;
                    else c->watch2 = k;

                    int new_idx = lit_index(l);

                    // add to new watch list
                    if (s->watch_sizes[new_idx] >= s->watch_cap[new_idx]) {
                        s->watch_cap[new_idx] = s->watch_cap[new_idx] ? s->watch_cap[new_idx] * 2 : 4;
                        s->watch_list[new_idx] = realloc(s->watch_list[new_idx], sizeof(int) * s->watch_cap[new_idx]);
                    }

                    s->watch_list[new_idx][s->watch_sizes[new_idx]++] = cid;

                    // remove from old watch list
                    s->watch_list[idx][i] = s->watch_list[idx][--s->watch_sizes[idx]];

                    moved = 1;
                    break;
                }
            }

            if (!moved) {
                int other_lit = c->lits[other_idx];
                int var = abs(other_lit);
                int val = s->assignment[var];

                if ((val == 1 && other_lit > 0) ||
                    (val == -1 && other_lit < 0)) {
                    i++; // clause already satisfied
                } else if (val == 0) {
                    // unit clause → force assignment
                    s->assignment[var] = (other_lit > 0) ? 1 : -1;
                    queue[(*qtail)++] = other_lit;
                    i++;
                } else {
                    return 0; // conflict
                }
            }
        }
    }

    return 1;
}