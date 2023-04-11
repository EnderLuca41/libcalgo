#include <stdlib.h>

void xor_encrypt_decrypt(char *data, size_t dataLen, char *key, size_t keyLen){
    for(size_t i = 0; i < dataLen; i++){
        data[i] ^= key[i % keyLen];
    }
}