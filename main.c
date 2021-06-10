#include "allocator.h"

int main(void) {
    for (int i = 0; i < 100000; ++i) {
        alloc(8);
    }

    return 0;
}