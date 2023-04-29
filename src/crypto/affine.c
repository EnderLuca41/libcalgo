#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static int mod(int x,int N){
    return (x % N + N) %N;
}

void affine_encrypt(const char *input, char *dest, const char *alphabet, int16_t a, int16_t b){
    char lookup[255];
    memset(lookup, -1, 255);
    size_t alphabetLen = strlen(alphabet);
    for(int i = 0; i < alphabetLen; i++){
        lookup[alphabet[i]] = i;
    }

    for(;*input != 0; input++){
        if(lookup[*input] == -1)
            continue; //Character isn't in alphabet, so it is ignored
        *dest = alphabet[mod(a * lookup[*input] + b, alphabetLen)];
        dest++;
    }
    *dest = 0;
}

bool affine_decrypt(const char *input, char *dest, const char *alphabet, int16_t a, int16_t b){
    size_t alphabetLen = strlen(alphabet);

    //Find multiplicative inverse
    short inverse = -1;
    for(int i = 0; i != INT16_MIN; i++) {
        if(mod(a * i, alphabetLen) == 1){
            inverse = i;
            break;
        }
    }

    if(inverse == -1)
        return false; //There is no multiplicative inverse for b

    char lookup[255];
    memset(lookup, -1, 255);
    for(int i = 0; i < alphabetLen; i++){
        lookup[alphabet[i]] = i;
    }

    for(;*input != 0; input++){
        *dest = alphabet[mod(inverse * (lookup[*input] - b), alphabetLen)];
        dest++;
    }
    *dest = 0;
    return true;
}