#include "allocator.h"

int main(void) {
    for (int i = 0; i < 100000; ++i) {
        cache_init(8, 8, 8, 8);
    }

    return 0;
}