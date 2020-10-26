#define I_MAX 1

#include "../include/knapsack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    clock_t start = clock();

    if (argc < 3) {
        puts("Uso: ./main FILE [GRASP | GRASP-PR]");
        exit(EXIT_FAILURE);
    }

    knapsack_read_file(argv[1]);
    srand((unsigned int) time(NULL));

    if (!strcmp(argv[2], "GRASP")) {
        knapsack_grasp(I_MAX);
    } else if (!strcmp(argv[2], "GRASP-PR")) {
        knapsack_grasp_path_relinking(I_MAX);
    } else {
        puts("Uso: ./main FILE [GRASP | GRASP-PR]");
        exit(EXIT_FAILURE);
    }

    clock_t end = clock();

    printf("Execution Time: %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}