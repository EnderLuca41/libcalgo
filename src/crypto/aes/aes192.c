#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "aesutil.h"

static void key_schedule(const uint8_t initKey[24], uint8_t outputKeys[13][16]){
    //Casting 2D array to 1D array, the output 13 * 16 wide but while calculating we use in theory a 9 * 24 array
    //Instead of working on a 9 * 24 array and later copying into the output we just cast and interpret the 1D array
    //dependent on the situation
    uint8_t *outputKeys1D = (uint8_t *) outputKeys;

    memcpy(outputKeys1D, initKey, 24);

    for(unsigned int i = 1; i < 9; i++){
        uint8_t lastColumn[4]; 

        for(unsigned j = 0; j < 4; j++){
            lastColumn[j] = outputKeys1D[(i-1) * 24 + j + 20];
        }

        rotword(lastColumn);
        subword(lastColumn);
        lastColumn[0] ^= Rc[i];

        for(unsigned int j = 0; j < 4; j++){
            outputKeys1D[i * 24 + j] = outputKeys1D[(i-1) * 24 + j] ^ lastColumn[j];
            outputKeys1D[i * 24 + j + 4] = outputKeys1D[(i-1) * 24 + j + 4] ^ outputKeys1D[i * 24 + j];
            outputKeys1D[i * 24 + j + 8] = outputKeys1D[(i-1) * 24 + j + 8] ^ outputKeys1D[i * 24 + j + 4];
            outputKeys1D[i * 24 + j + 12] = outputKeys1D[(i-1) * 24 + j + 12] ^ outputKeys1D[i * 24 + j + 8];
            if(i == 8) //13 * 16 is 2 32bit-words shorter than 9 * 24, so we have to cut them
                continue;
            outputKeys1D[i * 24 + j + 16] = outputKeys1D[(i-1) * 24 + j + 16] ^ outputKeys1D[i * 24 + j + 12];
            outputKeys1D[i * 24 + j + 20] = outputKeys1D[(i-1) * 24 + j + 20] ^ outputKeys1D[i * 24 + j + 16];
        }
    }
}

static void encrypt_block(uint8_t block[16], uint8_t keys[13][16]){
    //Initial Round
    for(unsigned int i = 0; i < 16; i++){
        block[i] ^= keys[0][i]; 
    }

    for(unsigned int i = 1; i < 12; i++){
        //SubBytes
        for(unsigned int j = 0; j < 16; j++){
            block[j] = S_Rijndael[block[j]];
        }

        shift_rows(block);       

        mix_columns(block);

        for(unsigned int j = 0; j < 16; j++){
            block[j] ^= keys[i][j];
        }
    }

    //Final Round
    for(unsigned int i = 0; i < 16; i++){
        block[i] = S_Rijndael[block[i]];
    }

    shift_rows(block);

    for(unsigned int i = 0; i < 16; i++){
        block[i] ^= keys[12][i];
    }
}

size_t aes192_encrypt_ECB(const void *input, void *dest, size_t inputSize, const uint8_t key[24]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    AesBuffer buffer;
    buffer.input = input;
    buffer.inputSize = inputSize;

    uint8_t block[16];
    size_t encrypted = 0;
    bool dataLeft;   
    do {
        dataLeft = generate_block(block, &buffer);
    
        encrypt_block(block, keys);

        memcpy(&((uint8_t *) dest)[encrypted], block, sizeof(block));
        encrypted += 16;
    } while (dataLeft);
    

    return encrypted;
}

size_t aes192_encrypt_CBC(const void *input, void *dest, size_t inputSize, const uint8_t key[24], const uint8_t initVector[16]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    AesBuffer buffer;
    buffer.input = input;
    buffer.inputSize = inputSize;

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    size_t encrypted = 0;
    bool dataLeft;   
    do {
        dataLeft = generate_block(block, &buffer);

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= lastBlock[i];
        }

        encrypt_block(block, keys);

        memcpy(lastBlock, block, 16);
        memcpy(&((uint8_t *) dest)[encrypted], block, sizeof(block));
        encrypted += 16;
    } while (dataLeft);
    

    return encrypted;
}

size_t aes192_encrypt_PCBC(const void *input, void *dest, size_t inputSize, const uint8_t key[24], const uint8_t initVector[16]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    AesBuffer buffer;
    buffer.input = input;
    buffer.inputSize = inputSize;

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    size_t encrypted = 0;
    bool dataLeft;   
    do {
        dataLeft = generate_block(block, &buffer);

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= lastBlock[i];
        }

        encrypt_block(block, keys);

        memcpy(lastBlock, input, 16);
        for(unsigned int i = 0; i < 16; i++){
            lastBlock[i] ^= block[i];
        }
        
        memcpy(&((uint8_t *) dest)[encrypted], block, sizeof(block));
        encrypted += 16;
    } while (dataLeft);
    

    return encrypted;
}

size_t aes192_encrypt_CTR(const void *input, void *dest, size_t inputSize, const uint8_t key[24], const uint8_t initVector[8]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    AesBuffer buffer;
    buffer.input = input;
    buffer.inputSize = inputSize;

    uint64_t counter = 0;
    uint8_t counterBlock[16];
    uint8_t block[16];
    size_t encrypted = 0;
    bool dataLeft;   
    do {
        dataLeft = generate_block(block, &buffer);

        memcpy(counterBlock, initVector, 8);
        memcpy(counterBlock + 8, &counter, 8);
        encrypt_block(counterBlock, keys);        

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= counterBlock[i];
        }
        
        memcpy(&((uint8_t *) dest)[encrypted], block, sizeof(block));
        encrypted += 16;
        counter++;
    } while (dataLeft);
    

    return encrypted;
}

size_t aes192_encrypt_CFB(const void *input, void *dest, size_t inputSize, const uint8_t key[24], const uint8_t initVector[16]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    AesBuffer buffer;
    buffer.input = input;
    buffer.inputSize = inputSize;

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    size_t encrypted = 0;
    bool dataLeft;   
    do {
        dataLeft = generate_block(block, &buffer);

        encrypt_block(lastBlock, keys);        

        for(unsigned int i = 0; i < 16; i++){
            lastBlock[i] ^= block[i];
        }   

        memcpy(&((uint8_t *) dest)[encrypted], lastBlock, sizeof(block));
        encrypted += 16;
    } while (dataLeft);
    

    return encrypted;
}

static void decrypt_block(uint8_t block[16], uint8_t keys[13][16]){
    for(unsigned int i = 0; i < 16; i++) {
        block[i] ^= keys[12][i];
    }

    undo_shift_rows(block);

    for(unsigned int i = 0; i < 16; i++){
        block[i] = S_RijndaelInverse[block[i]];
    }

    for(unsigned int i = 11; i > 0; i--){
        for(unsigned int j = 0; j < 16; j++){
            block[j] ^= keys[i][j];
        }

        undo_mix_columns(block);

        undo_shift_rows(block);

        for(unsigned int i = 0; i < 16; i++){
            block[i] = S_RijndaelInverse[block[i]];
        }
    }

    for(unsigned int i = 0; i < 16; i++) {
        block[i] ^= keys[0][i];
    }
}

void aes192_decrypt_ECB(const void *input, void *dest, size_t inputSize, uint8_t key[24]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    uint8_t block[16];
    while(inputSize != 0) {
        memcpy(block, input, sizeof(block));

        decrypt_block(block, keys);

        memcpy(dest, block, sizeof(block));
        inputSize -= 16;
        dest += 16;
        input += 16;
    }
}

void aes192_decrypt_CBC(const void *input, void *dest, size_t inputSize, uint8_t key[24], const uint8_t initVector[16]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    while(inputSize != 0) {
        memcpy(block, input, sizeof(block));

        decrypt_block(block, keys);

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= lastBlock[i];
        }

        memcpy(dest, block, sizeof(block));
        memcpy(lastBlock, input, sizeof(lastBlock));
        inputSize -= 16;
        dest += 16;
        input += 16;
    }
}

void aes192_decrypt_PCBC(const void *input, void *dest, size_t inputSize, uint8_t key[24], const uint8_t initVector[16]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    while(inputSize != 0) {
        memcpy(block, input, sizeof(block));

        decrypt_block(block, keys);

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= lastBlock[i];
        }

        memcpy(lastBlock, input, 16);
        for(unsigned int i = 0; i < 16; i++){
            lastBlock[i] ^= block[i];
        }
       
        memcpy(dest, block, sizeof(block));
        inputSize -= 16;
        dest += 16;
        input += 16;
    }
}

void aes192_decrypt_CTR(const void *input, void *dest, size_t inputSize, uint8_t key[24], const uint8_t initVector[8]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    uint64_t counter = 0;
    uint8_t counterBlock[16];
    uint8_t block[16];
    while(inputSize != 0) {
        memcpy(block, input, sizeof(block));

        memcpy(counterBlock, initVector, 8);
        memcpy(counterBlock + 8, &counter, 8);
        encrypt_block(counterBlock, keys);    

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= counterBlock[i];
        }
       
        memcpy(dest, block, sizeof(block));
        inputSize -= 16;
        dest += 16;
        input += 16;
        counter++;
    }
}

void aes192_decrypt_CFB(const void *input, void *dest, size_t inputSize, uint8_t key[24], const uint8_t initVector[16]){
    uint8_t keys[13][16];
    key_schedule(key, keys);

    AesBuffer buffer;
    buffer.input = input;
    buffer.inputSize = inputSize;

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    while(inputSize != 0) {
        memcpy(block, input, sizeof(block));

        encrypt_block(lastBlock, keys);    

        for(unsigned int i = 0; i < 16; i++){
            lastBlock[i] ^= block[i];
        }
       
        memcpy(dest, lastBlock, sizeof(block));
        memcpy(lastBlock, block, sizeof(lastBlock));
        inputSize -= 16;
        dest += 16;
        input += 16;
    }
}