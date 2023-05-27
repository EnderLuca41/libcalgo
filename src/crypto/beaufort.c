#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static int mod(int x,int N){
    return (x % N + N) %N;
}

//Returns false if alphabet contains a non ascii character
bool beaufort_encrypt_decrypt(char *text,  const char *key, const char *alphabet){
    size_t keyLen = strlen(key);
    size_t alphabetLen = strlen(alphabet);

    //Create lookup table for the position of the characters
    unsigned char lookup[128];
    memset(lookup, 255, 128);
    for(unsigned char i = 0; i < alphabetLen; i++){
        if(alphabet[i] < 0)
            return false;
        lookup[alphabet[i]] = i;
    }

    unsigned char j = 0; //Index of key
    for(size_t i = 0; text[i] != '\0'; i++){
        if(lookup[text[i]] == 255)
            continue;

        text[i] = alphabet[mod(lookup[key[j]] - lookup[text[i]], alphabetLen)];
        j = (j + 1) % keyLen;
    }

    return true;
}