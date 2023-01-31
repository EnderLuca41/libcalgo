#include <stdint.h>
#include <stdlib.h>

//Casts d to a 16 bit uint pointer, and then derefrences that pointer to get a 16bit uint value
#define get16bits(d) (*((const uint16_t *) (d)))

uint32_t super_fast_hash(const uint8_t *input, int len){
    uint32_t hash = (uint32_t) len, tmp;
    int rem;

    rem = len & 3;
    len >>= 2;

    for(;len > 0; len--){
        hash += get16bits(input);
        tmp = (uint32_t) (get16bits(input + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        input +=  2 * sizeof(uint16_t); //4
        hash += hash >> 11;
    }
    
    //Processes 1-3 bytes if they are left
    switch (rem)
    {
        case 3: 
            hash += get16bits(input);
            hash ^= hash << 16;
            //Get the last byte, move it 18 bit to the left, and cast it to a uint32_t
            hash ^= (uint32_t) (((signed char) input[sizeof(uint16_t)]) << 18); 
            hash += hash >> 11;
            break;
        
        case 2:
            hash += get16bits(input);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;

        case 1:
            hash += (uint32_t) ((signed char) *input);
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}