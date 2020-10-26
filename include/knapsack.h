#ifndef AA3_KNAPSACK_H
#define AA3_KNAPSACK_H

#include <stdbool.h>

typedef struct object_t {
    int index;
    int weight;
    int value;
    double profit; // (value / weight)
} object_t;

void knapsack_read_file(const char *filepath);
void knapsack_greedy_randomized_construction(bool *x);
void knapsack_local_search(bool *x);
void knapsack_grasp(int i_max);
void knapsack_path_relinking(bool *x, bool *xt);
void knapsack_grasp_path_relinking(int i_max);

#endif //AA3_KNAPSACK_H
