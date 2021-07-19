#include "allocator.h"
#include <stdlib.h>
#include <time.h>

int main(void) {

    for (int i = 0; i < 100; ++i) {
        alloc(2);
    }

    return 0;
    alloc(2);
    malloc(2);
    for (int k = 0; k < 1; ++k) {
        void* addresses[100];
        clock_t start = clock();

        for (int i = 0; i < 100; ++i) {
            addresses[i] = alloc(2);
        }

        printf("\nalloc(): done in %0.9LF secs.", (long double)(clock() - start) / (long double)CLOCKS_PER_SEC);

        void* maddresses[100];

        clock_t mstart = clock();

        for (int i = 0; i < 100; ++i) {
            maddresses[i] = malloc(2);
        }

        printf("\nmalloc(): done in %0.9LF secs.", (long double)(clock() - mstart) / (long double)CLOCKS_PER_SEC);

        putchar('\n');
    }

    return 0;
}