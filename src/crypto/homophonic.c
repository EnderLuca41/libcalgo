#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

//Information about the capitalization gets lost in the encyption process
void homophonic_cipher_encrypt(char *data, const char key[26][6], const uint8_t subkeysLen[26]){
    srand(time(NULL));
    for(; *data != 0; data++){
        if(islower(*data)){
            *data = key[*data - 97][rand() % subkeysLen[*data - 97]];
            continue;
        }

        if(isupper(*data)){
            *data = key[*data - 65][rand() % subkeysLen[*data - 65]];
        }
    }
}

void homophonic_cipher_decrypt(char *data, const char key[26][6], const uint8_t subkeysLen[26]){

    //Create a loopup table for the key to trade space for speed
    char lookup[255];
    memset(lookup, 0, 255);
    for(int i = 0; i < 26; i++){
        for(int j = 0; j < subkeysLen[i]; j++){
            lookup[(unsigned char) key[i][j]] =  'A' + i;
        }
    }

    for(; *data != 0; data++){
        char e = lookup[(unsigned char) *data];
        if(e == 0)
            continue;
        *data = e;
    }   
}