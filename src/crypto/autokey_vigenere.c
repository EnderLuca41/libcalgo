#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

static int mod(int x,int N){
    return (x % N + N) %N;
}

void autokey_vigenere_encrypt(const char *input, char *dest, const char *key){
    size_t keyLen = strlen(key);

    char *buffer = malloc(keyLen); //buffers the last input to use as a key
    int64_t buffPtr = 0;
    size_t encryptedChars = 0;
    size_t i = 0;
    for(; input[i] != 0; i++){
        if(isalpha(input[i])){
            if(keyLen > encryptedChars){
                char keyChar = toupper(key[encryptedChars]) - 65;
                dest[i] = (keyChar + (toupper(input[i]) - 65)) % 26 + 65;
                buffer[buffPtr] = input[i];
                buffPtr = (buffPtr + 1) % keyLen;
                encryptedChars++;
            }
            else {              
                char keyChar = toupper(buffer[mod((buffPtr), keyLen)]) - 65;  
                dest[i] = (keyChar + (toupper(input[i]) - 65)) % 26 + 65;
                buffer[buffPtr] = input[i];
                buffPtr = (buffPtr + 1) % keyLen;
                encryptedChars++;
            }

            continue;
        }

        dest[i] = input[i];
    }
    dest[i] = 0;

    free(buffer);
}

void autokey_vigenere_decrypt(const char *input, char *dest, const char *key){
    size_t keyLen = strlen(key);

    char *buffer = malloc(keyLen); //buffers the last decrypted input to use as a key
    int64_t buffPtr = 0;
    size_t decryptedChars = 0;
    size_t i = 0;
    for(; input[i] != 0; i++){
        if(isalpha(input[i])){
            if(keyLen > decryptedChars){ 
                char keyChar = toupper(key[decryptedChars]) - 65;
                dest[i] = mod((input[i] - 65) - keyChar, 26) + 65;
                buffer[buffPtr] = dest[i];
                buffPtr = (buffPtr + 1) % keyLen;
                decryptedChars++;
            }
            else {
                char keyChar = toupper(buffer[mod(buffPtr, keyLen)]) - 65;
                dest[i] = mod((input[i] - 65) - keyChar, 26) + 65;
                buffer[buffPtr] = dest[i];
                buffPtr = (buffPtr + 1) % keyLen;
                decryptedChars++;
            }

            continue;
        }

        dest[i] = input[i];
    }
    dest[i] = 0;

    free(buffer);
}
