#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static void lower_string(char *text){
    for(;*text != 0; text++)
        *text = tolower(*text);
}

void vigenere_cipher_encrypt(char *text, const char *key){
    int keylen = strlen(key);
    char* keylower = malloc(keylen);
    memcpy(keylower, key, keylen);
    lower_string(keylower);

    for(size_t i = 0; text[i] != 0; i++){
        if(isupper(text[i])){
            text[i] += (keylower[i % keylen] - 97);
            if(text[i] > 'Z')
                text[i] -= 26;
            continue;
        }

        if(islower(text[i])){
            text[i] += (keylower[i % keylen] - 97);
            if(text[i] > 'z')
                text[i] -= 26;
        }
    }

    free(keylower);
}

void vigenere_cipher_decrypt(char *text, const char *key){
    int keylen = strlen(key);
    char* keylower = malloc(keylen);
    memcpy(keylower, key, keylen);
    lower_string(keylower);

    for(size_t i = 0; text[i] != 0; i++){
        if(isupper(text[i])){
            text[i] -= (keylower[i % keylen] - 97);
            if(text[i] < 'A')
                text[i] += 26;
        }

        if(islower(text[i])){
            text[i] -= (keylower[i % keylen] - 97);
            if(text[i] < 'a')
                text[i] += 26;
        }
    }

    free(keylower);
}