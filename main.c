#include "random.h"
#include "source/allocator.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    for (int i = 0; i < 4; ++i) {
        clock_t astart = clock();

        for (int f = 0; f < 1000000; ++f) {
            alloc(2);
        }

        printf("\nalloc(): done in %0.8LF secs.", (long double)(clock() - astart) / (long double)CLOCKS_PER_SEC);

        clock_t mstart = clock();

        for (int f = 0; f < 1000000; ++f) {
            malloc(2);
        }

        printf("\nmalloc(): done in %0.8LF secs.\n", (long double)(clock() - mstart) / (long double)CLOCKS_PER_SEC);
    }

    return 0;
}