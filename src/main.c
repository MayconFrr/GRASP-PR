#define I_MAX 100

#include "../include/knapsack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    clock_t start, end;
    double time_sum = 0;
    long long f_sum = 0;

    if (argc < 3) {
        puts("Uso: ./main FILE [GRASP | GRASP-PR]");
        exit(EXIT_FAILURE);
    }

    knapsack_read_file(argv[1]);
    srand((unsigned int) time(NULL));

    for (int i = 0; i < 5; i++) {
        if (!strcmp(argv[2], "GRASP")) {
            start = clock();
            f_sum += knapsack_grasp(I_MAX);
            end = clock();
        } else if (!strcmp(argv[2], "GRASP-PR")) {
            start = clock();
            f_sum += knapsack_grasp_path_relinking(I_MAX);
            end = clock();
        } else {
            puts("Usage: ./main FILE [GRASP | GRASP-PR]");
            exit(EXIT_FAILURE);
        }
        time_sum += (double) (end - start) / CLOCKS_PER_SEC;
    }

    printf("Average f(x): %f\n"
           "Average execution time: %lfs\n", (double) f_sum / 5.0, time_sum / 5.0);

    return 0;
}