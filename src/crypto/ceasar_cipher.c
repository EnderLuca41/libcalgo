#include <ctype.h>
#include <stdlib.h>

void ceasar_cipher_encrypt(char *text, unsigned int key){
    key %= 26;
    for(size_t i = 0; text[i] != 0; i++){
        if(isupper(text[i])){
            text[i] += key;
            if(text[i] > 'Z')
                text[i] -= 26;
            continue;
        }

        if(islower(text[i])){
            text[i] += key;
            if(text[i] > 'z')
                text[i] -= 26;
        }
    }
}

void ceasar_cipher_decrypt(char *text, unsigned int key){
    key %= 26;
    for(size_t i = 0; text[i] != 0; i++){
        if(isupper(text[i])){
            text[i] -= key;
            if(text[i] < 'A')
                text[i] += 26;
            continue;
        }

        if(islower(text[i])){
            text[i] -= key;
            if(text[i] < 'a')
                text[i] += 26;
        }
            
    }
}
