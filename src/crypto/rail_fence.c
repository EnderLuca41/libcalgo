#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void rail_fence_encrypt(const char *input, char *dest, uint32_t key){
    size_t inputLen = strlen(input);
    uint32_t maxDistance = key * 2 - 3;


    uint32_t a = maxDistance-1;
    uint32_t b = 2;
    for(uint32_t i = 0; i < inputLen; i += maxDistance+1){
        *dest = input[i];
        dest++;
    }

    for(uint32_t i = 1; i < key-1; i++){

        int toAdd = -1;
        for(uint32_t j = i; j < inputLen;){
            *dest = input[j];
            if(toAdd == -1)
                j += a;
            else
                j += b;

            toAdd = -toAdd;
            dest++;
        }

        a-=2;
        b+=2;
    }

    for(uint32_t i = maxDistance / 2 + 1; i < inputLen; i += maxDistance+1){
        *dest = input[i];
        dest++;
    }
    *dest = 0;
}

void rail_fence_decrypt(const char *input, char *dest, uint32_t key){
    size_t inputLen = strlen(input);
    uint32_t maxDistance = key * 2 - 3;

    uint32_t a = maxDistance-1;
    uint32_t b = 2;
    for(uint32_t i = 0; i < inputLen; i += maxDistance+1){
        dest[i] = *input;
        input++;
    }

    for(uint32_t i = 1; i < key-1; i++){

        int toAdd = -1;
        for(uint32_t j = i; j < inputLen;){
            dest[j] = *input;
            if(toAdd == -1)
                j += a;
            else
                j += b;

            toAdd = -toAdd;
            input++;
        }

        a-=2;
        b+=2;
    }

    for(uint32_t i = maxDistance / 2 + 1; i < inputLen; i += maxDistance+1){
        dest[i] = *input;
        input++;
    }
    dest[inputLen] = 0;
}