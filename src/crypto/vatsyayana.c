#include <ctype.h>

//Letters in key must be uppercase
void vatsyayana_cipher_encrypt_decrypt(char *data, const char key[13][2]){

    //Create a looklup Table to speed things up
    char lookupTable[26];
    for(int i = 0; i < 13; i++){
        lookupTable[key[i][0] - 65] = key[i][1];
        lookupTable[key[i][1] - 65] = key[i][0];
    }

    for(;*data != 0; data++) {
        if(isupper(*data)){
            *data = lookupTable[*data - 65];
            continue;
        }   

       if(islower(*data)){
            *data = lookupTable[*data - 97] + 32;
        }
    }
}