#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "blowfishutil.h"

/*
 * Key length can 32-448 bit (4-56 bytes)
 * returns: Size of encrypted bytes in dest
 */
size_t blowfish_encrypt(const void *input, void *dest, size_t inputSize, const uint8_t *key, uint8_t keyLen){
    if(keyLen < 4 || keyLen > 56)
        return false;

    BlowfishContext ctx;
    blowfish_init(key, keyLen, &ctx);

    uint32_t l, r;
    size_t encrypted = 0;
    while(inputSize != 0){
        inputSize = generate_block_blowfish(&l, &r, input, inputSize);
        encrypt_block(&l, &r, &ctx);
        memcpy(&((uint8_t *) dest)[encrypted], &l, sizeof(uint32_t));
        memcpy(&((uint8_t *) dest)[encrypted] + 4, &r, sizeof(uint32_t));

        encrypted += 8;
        input += 8;
    }
}

/*
 * Key length can 32-448 bit (4-56 bytes)
 * return: value indicates if decryption was successful
 */
bool blowfish_decrypt(const void *input, void *dest, size_t inputSize, const uint8_t *key, uint8_t keyLen){
    if(keyLen < 4 || keyLen > 56)
        return false;

    BlowfishContext ctx;
    blowfish_init(key, keyLen, &ctx);
    
    uint32_t l, r;
    while(inputSize != 0){
        memcpy(&l, input, sizeof(l));
        memcpy(&r, input + 4, sizeof(r));

        decrypt_block(&l, &r, &ctx);
        
        for(unsigned int i = 0; i < 4; i++){
            ((uint8_t *)dest)[i] = (uint8_t) (l >> (24 - i * 8));
        }
        
        for(unsigned int i = 0; i < 4; i++){
            ((uint8_t *)dest)[i + 4] = (uint8_t) (r >> (24 - i * 8));
        }

        dest += 8;
        input += 8;
        inputSize -= 8;
    }
}