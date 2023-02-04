#include <stdlib.h>
#include <stdint.h>

uint32_t one_at_a_time(const uint8_t *input, size_t size){
    uint32_t hash = 0;

    for(int i; i < size;) {
        hash += input[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}
