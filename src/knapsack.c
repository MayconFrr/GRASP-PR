#define ALPHA 0.5
#define ELITE_MAX 10

#define max(a, b) ((a) > (b) ? (a) : (b))

#define STB_DS_IMPLEMENTATION

#include "../include/knapsack.h"
#include "../include/stb_ds.h"

#include <limits.h>
#include <stdio.h>

/* Private variables */

int n = 0;  // Number of objects
int w_max = 0;  // Max weight of the knapsack
object_t *objects = NULL;   // Array of objects

/* Private functions */

// Auxiliar function to create change a boolean bit
void change_bit(bool *bit) {
    *bit = *bit ? false : true;
}

// Calculates the function of a given solution array x
long long f(const bool *x) {
    long long penalty = 0;
    long long value = 0;
    long long weight = 0;

    for (int i = 0; i < n; ++i) {
        if (x[i]) {
            value += objects[i].value;
            weight += objects[i].weight;
        }
        penalty += objects[i].weight;
    }

    return value - penalty * max(0, weight - w_max);
}

// Calculates the weight used in knapsack for a given solution x
int w(const bool *x) {
    int w = 0;

    for (int i = 0; i < n; ++i) {
        if (x[i]) {
            w += objects[i].weight;
        }
    }

    return w;
}

/*
 * Inserts a solution x in the elite set if there is space in the set or
 * if it's better than the worst solution in the set.
 */
bool **elite_put(bool **elite, bool *x) {
    bool *candidate = malloc(n * sizeof(bool));
    memcpy(candidate, x, n * sizeof(bool));

    if (arrlen(elite) < arrcap(elite)) {
        arrput(elite, candidate);
    } else {
        long long int f_min = LLONG_MAX;
        int worst_index = -1;

        for (int i = 0; i < arrlen(elite); ++i) {
            if (f(elite[i]) < f_min) {
                f_min = f(elite[i]);
                worst_index = i;
            }
        }

        if (f(candidate) > f_min) {
            free(elite[worst_index]);
            elite[worst_index] = candidate;
        }
    }

    // Pointer may be reallocated
    return elite;
}

// Compare function to be used with qsort
int profit_cmp(const void *o1, const void *o2) {
    double p1 = ((object_t *) o1)->profit;
    double p2 = ((object_t *) o2)->profit;

    if (p1 < p2) {
        return 1;
    } else if (p1 > p2) {
        return -1;
    } else {
        return 0;
    }
}

// Calculates what indexes of x are different from y
int *symmetric_difference(const bool *x, const bool *y) {
    int *delta = NULL;

    for (int i = 0; i < n; ++i) {
        if (y[i] != x[i]) {
            arrput(delta, i);
        }
    }

    return delta;
}

/* Public functions */

// Reads the file specified by filepath and sets global variables
void knapsack_read_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d %d\n", &n, &w_max) < 2) {
        exit(EXIT_FAILURE);
    }

    objects = malloc(n * sizeof(object_t));

    for (int i = 0; i < n; ++i) {
        if (fscanf(file, "%d %d\n", &objects[i].value, &objects[i].weight) < 2) {
            exit(EXIT_FAILURE);
        }
        objects[i].profit = (double) objects[i].value / objects[i].weight;
        objects[i].index = i;
    }

    fclose(file);
}

// Constructs a greedy randomized solution in x
void knapsack_greedy_randomized_construction(bool *x) {
    int weight = 0L;

    // Clear solution
    memset(x, 0, n * sizeof(bool));

    // Create sorted objects list
    object_t *sorted_objects = NULL;
    arrsetlen(sorted_objects, n);

    // Sorted copy of objects by profit
    memcpy(sorted_objects, objects, n * sizeof(object_t));
    qsort(sorted_objects, n, sizeof(object_t), profit_cmp);

    while (arrlen(sorted_objects) > 0 && weight < w_max) {
        int rcl_size = 0U;

        object_t obj1 = sorted_objects[0];
        object_t obj2 = arrlast(sorted_objects);

        for (int i = 0; i < arrlen(sorted_objects); ++i) {
            object_t obj = sorted_objects[i];

            if (obj.profit >= (obj1.profit - ALPHA * (obj1.profit - obj2.profit))) {
                ++rcl_size;
            } else {
                break;
            }
        }

        int index = rand() % rcl_size;
        object_t obj = sorted_objects[index];

        if (!x[obj.index] && weight + obj.weight <= w_max) {
            x[obj.index] = true;
            weight += obj.weight;
        }

        arrdel(sorted_objects, index);
    }

    // Free allocated resources
    arrfree(sorted_objects);
}

 /*
  * Conducts a local search from x while a better neighbor can be found;
  *
  * The neighbor n with the best f(n) is chosen each iteration
  */
void knapsack_local_search(bool *x) {
    long long f_star = f(x);
    bool done = false;

    do {
        long long f_max = LLONG_MIN;
        int best_index = -1;

        for (int i = 0; i < n; ++i) {
            change_bit(&x[i]);

            if (f(x) > f_max) {
                best_index = i;
                f_max = f(x);
            }

            change_bit(&x[i]);
        }
        if (f_max > f_star) {
            change_bit(&x[best_index]);
            f_star = f_max;
        } else {
            done = true;
        }
    } while (!done);
}

/*
 * Creates a greedy randomized solution x applies a local search on it i_max times;
 *
 * Returns the function of the best solution
 */
long long knapsack_grasp(int i_max) {
    long long f_star = LLONG_MIN;

    bool *x_star = malloc(n * sizeof(bool));
    bool *x = malloc(n * sizeof(bool));

    for (int i = 0; i < i_max; ++i) {
        knapsack_greedy_randomized_construction(x);
        knapsack_local_search(x);

        if (f(x) > f_star) {
            f_star = f(x);
            x_star = memcpy(x_star, x, n * sizeof(bool));
        }
    }

    // Free allocated resources
    free(x_star);
    free(x);

    return f_star;
}

// Finds the neighbor in the path between x and xt and saves it in x
void knapsack_path_relinking(bool *x, bool *xt) {
    long long f_star;
    bool *x_star = malloc(n * sizeof(bool));

    if (f(x) > f(xt)) {
        f_star = f(x);
        x_star = memcpy(x_star, x, n * sizeof(bool));
    } else {
        f_star = f(xt);
        x_star = memcpy(x_star, xt, n * sizeof(bool));
    }

    int *delta = symmetric_difference(x, xt);

    while (arrlen(delta) > 0) {
        long long int f_max = LLONG_MIN;
        int best_index = -1;

        for (int i = 0; i < arrlen(delta); ++i) {
            change_bit(&x[delta[i]]);

            if (f(x) > f_max) {
                best_index = i;
                f_max = f(x);
            }

            change_bit(&x[delta[i]]);
        }

        change_bit(&x[delta[best_index]]);

        arrdelswap(delta, best_index);

        if (f(x) > f_star) {
            f_star = f(x);
            x_star = memcpy(x_star, x, n * sizeof(bool));
        }
    }

    memcpy(x, x_star, n * sizeof(bool));

    // Free allocated resources
    arrfree(delta);
    free(x_star);
}

/*
 * Creates a greedy randomized solution x and applies a local search on it i_max times;
 * The best solutions are saved in a elite set with ELITE_MAX capacity;
 *
 * After the first iteration a path-relinking algorithm is applied between the constructed
 * solution and each of the solutions in the elite set as an intensification strategy;
 *
 * Returns the function of the best solution
 */
long long knapsack_grasp_path_relinking(int i_max) {
    bool *x_star = malloc(n * sizeof(bool));
    bool *x = malloc(n * sizeof(bool));
    bool **elite = NULL;
    long long f_star = LLONG_MIN;

    arrsetcap(elite, ELITE_MAX);

    for (int i = 0; i < i_max; ++i) {
        knapsack_greedy_randomized_construction(x);
        knapsack_local_search(x);

        if (i > 0) {
            for (int j = 0; j < arrlen(elite); ++j) {
                knapsack_path_relinking(x, elite[j]);
            }
        }

        if (f(x) > f_star) {
            f_star = f(x);
            x_star = memcpy(x_star, x, n * sizeof(bool));
        }

        elite = elite_put(elite, x_star);
    }

    // Free allocated resources
    for (int i = 0; i < arrlen(elite); ++i) {
        free(elite[i]);
    }
    arrfree(elite);
    free(x_star);
    free(x);

    return f_star;
}
