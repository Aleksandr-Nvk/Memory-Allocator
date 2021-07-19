#include "allocator.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(void) {
    for (int i = 0; i < 4; ++i) {
        clock_t astart = clock();

        for (int f = 0; f < 10000000; ++f) {
            alloc(i);
        }

        printf("\nalloc(): done in %0.8LF secs.", (long double)(clock() - astart) / (long double)CLOCKS_PER_SEC);

        clock_t mstart = clock();

        for (int f = 0; f < 10000000; ++f) {
            malloc(i);
        }

        printf("\nmalloc(): done in %0.8LF secs.\n", (long double)(clock() - mstart) / (long double)CLOCKS_PER_SEC);
    }

    return 0;
}