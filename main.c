#include "random.h"
#include "source/allocator.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define SIZE    100000

/* tests */
int main(void) {

    for (int p = 0; p < 4; ++p) {
        void* saddresses[SIZE];
        void* maddresses[SIZE];

        for (int i = 0; i < SIZE; ++i) {
            saddresses[i] = alloc(1);
            maddresses[i] = malloc(1);
        }

        for (int i = 0; i < SIZE / 2; ++i) {
            size_t index = rand_int_range(0, SIZE - 1);

            if (saddresses[index] != NULL) {
                sfree(saddresses[index]);
                free(maddresses[index]);
                saddresses[index] = NULL;
                maddresses[index] = NULL;
            } else {
                continue;
            }
        }

        clock_t astart = clock();

        for (int f = 0; f < SIZE; ++f) {
            if (saddresses[f] != NULL) {
                sfree(saddresses[f]);
            }
        }

        printf("\nalloc(): done in %0.8LF secs.", (long double)(clock() - astart) / (long double)CLOCKS_PER_SEC);

        clock_t mstart = clock();

        for (int f = 0; f < SIZE; ++f) {
            if (maddresses[f] != NULL) {
                free(maddresses[f]);
            }
        }

        printf("\nmalloc(): done in %0.8LF secs.\n", (long double)(clock() - mstart) / (long double)CLOCKS_PER_SEC);
    }

    return 0;
}