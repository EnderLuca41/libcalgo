#include <ctype.h>

//The alphabet from key has to be uppercase
void mono_alphabetic_cipher_encrypt(char *input, char key[26]){
    for(;*input != 0; input++){
        if(isupper(*input)){
            *input = key[*input - 65];  
            continue;
        }

        if(islower(*input)){
            *input = tolower(key[*input - 97]);
        }
    }
}

//The alphabet from key has to be uppercase
void mono_alphabetic_cipher_decrypt(char *input, char key[26]){

    //Instead of searching the array everytime for the index of the character,
    //we just create a lookup table to speed things up 
    char lookupTable[26];
    for(int i = 0; i < 26; i++){
        lookupTable[key[i] - 65] = i;
    }

    for(;*input != 0; input++){
        if(isupper(*input)){
            *input = lookupTable[*input - 65] + 65;
            continue;
        }

        if(islower(*input)){
            *input = lookupTable[*input - 97] + 97;
        }
    }
}