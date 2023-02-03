#ifndef HASHING_H
#define HASHING_H

#include <stdint.h>
#include <stdlib.h>

//Struct used as buffer for the buzhash algorithm, DON'T MESS AROUND WITH THE STRUCT MEMBERS
typedef struct {
    uint32_t __hash;
    uint32_t __blockSize;
    uint8_t *__buf;
    uint32_t __bufpos;
    bool __overflow;
    uint8_t __bshift;
    const uint32_t *__bytehash;
} Buzhash;
/*
#ifdef __cplusplus
extern "C" {
#endif*/

//Get the current hash
uint32_t buzhash_get(Buzhash *bh);
//Resets the algorithm to the state at the beginning
void buzhash_reset(Buzhash *bh);
//Creates an instance of the buzhash algorithm
Buzhash * buzhash_create(uint32_t blockSize, uint32_t *bytehash);
//Destroys the instace of the buzhash algorithm and free's all allocated memory
void buzhash_destoy(Buzhash *bh);
//Update the hash with new bytes
void buzhash_write(Buzhash *bh, const uint8_t *input, size_t len);
/*
#ifdef __cplusplus
}
#endif*/
#endif