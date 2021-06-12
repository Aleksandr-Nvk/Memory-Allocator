#include "allocator.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    for (int i = 0; i < 1000000; ++i) {
        alloc(1);
    }

    return 0;
    for (int k = 0; k < 5; ++k) {
        void* addresses[1000000];
        clock_t start = clock();

        for (int i = 0; i < 1000000; ++i) {
            addresses[i] = alloc(4);
        }

        printf("\nalloc(): done in %LF secs.", (long double)(clock() - start) / (long double)CLOCKS_PER_SEC);

        void* maddresses[1000];

        clock_t mstart = clock();

        for (int i = 0; i < 1000; ++i) {
            maddresses[i] = malloc(4);
        }

        printf("\nmalloc(): done in %LF secs.", (long double)(clock() - mstart) / (long double)CLOCKS_PER_SEC);

        putchar('\n');
    }

    return 0;
}