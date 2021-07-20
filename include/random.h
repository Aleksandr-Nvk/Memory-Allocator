#include <stdint.h>

                                        /* *** UNSIGNED 64-BIT INTEGER *** */

/* basic random uint64_t stream structure */
typedef struct Random64 {
    uint64_t seed;
    uint64_t current;
} Random64;

/* returns a new uint64_t stream seeded with 'seed' */
Random64 new_rand64(uint64_t seed);

/* returns the next uint64_t value from 'stream' */
uint64_t next64(Random64* stream);

/* returns a random uint64_t value */
uint64_t rand64(void);

                                        /* *** UNSIGNED 32-BIT INTEGER *** */

/* basic random uint32_t stream structure */
typedef struct Random32 {
    uint32_t seed;
    uint32_t current;
} Random32;

/* returns a new uint32_t stream seeded with 'seed' */
Random32 new_rand32(uint32_t seed);

/* returns the next uint32_t value from 'stream' */
uint32_t next32(Random32* stream);

/* returns a random uint32_t value */
uint32_t rand32(void);

                                        /* *** UNSIGNED 16-BIT INTEGER *** */

/* basic random uint16_t stream structure */
typedef struct Random16 {
    uint16_t seed;
    uint16_t current;
} Random16;

/* returns a new uint16_t stream seeded with 'seed' */
Random16 new_rand16(uint16_t seed);

/* returns the next uint16_t value from 'stream' */
uint16_t next16(Random16* stream);

/* returns a random uint16_t value */
uint16_t rand16(void);

                                        /* *** UNSIGNED 8-BIT INTEGER *** */

/* basic random uint8_t stream structure */
typedef struct Random8 {
    uint8_t seed;
    uint8_t current;
} Random8;

/* returns a new uint8_t stream seeded with 'seed' */
Random8 new_rand8(uint8_t seed);

/* returns the next uint8_t value from 'stream' */
uint8_t next8(Random8* stream);

/* returns a random uint8_t value */
uint8_t rand8(void);

                                        /* *** BOOL (0 or 1) *** */

/* basic random bool stream structure */
typedef struct RandomBool {
    uint8_t seed;
    uint8_t current;
} RandomBool;

/* returns a new bool stream seeded with 'seed' */
RandomBool new_rand_bool(uint8_t seed);

/* returns the next bool value (0 or 1) from 'stream' */
uint8_t next_bool(RandomBool* stream);

/* returns either 0 or 1 randomly */
uint8_t rand_bool(void);

                                        /* *** ADDITIONAL HANDY FUNCTIONS *** */

/* returns a random int64_t value between min inclusive and max inclusive */
int64_t rand_int_range(int64_t min, int64_t max);

/* returns a random long double value between min inclusively and max inclusively */
long double rand_l_double_range(long double min, long double max);

/* returns a random double value between min inclusively and max inclusively */
double rand_double_range(double min, double max);

/* returns a random float value between min inclusively and max inclusively */
float rand_float_range(float min, float max);