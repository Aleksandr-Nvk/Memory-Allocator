#include "allocator.h"
#include <stdlib.h>
#include <time.h>

int main(void) {
    for (int k = 0; k < 5; ++k) {
        void* addresses[100000];
        clock_t start = clock();

        for (int i = 0; i < 100000; ++i) {
            addresses[i] = alloc(8);
        }

        printf("\nalloc(): done in %lf secs.", (double)(clock() - start) / (double)CLOCKS_PER_SEC);
        
        void* maddresses[100000];

        clock_t mstart = clock();

        for (int i = 0; i < 100000; ++i) {
            maddresses[i] = malloc(8);
        }

        printf("\nmalloc(): done in %lf secs.", (double)(clock() - mstart) / (double)CLOCKS_PER_SEC);

        putchar('\n');
    }

    return 0;
}