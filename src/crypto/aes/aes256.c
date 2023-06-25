#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "aesutil.h"


void key_schedule(const uint8_t initKey[32], uint8_t outputKeys[15][16]){
    //Casting 2D array to 1D array, the output 15 * 16 wide but while calculating we use in theory a 9 * 24 array
    //Instead of working on a 8 * 32 array and later copying into the output we just cast and interpret the 1D array
    //dependent on the situation
    uint8_t *outputKeys1D = (uint8_t *) outputKeys;

    memcpy(outputKeys1D, initKey, 32);

    for(unsigned int i = 1; i < 8; i++){
        uint8_t lastColumn[4]; 

        for(unsigned j = 0; j < 4; j++){
            lastColumn[j] = outputKeys1D[(i-1) * 32 + j + 28];
        }

        rotword(lastColumn);
        subword(lastColumn);
        lastColumn[0] ^= Rc[i];

        for(unsigned int j = 0; j < 4; j++){
            outputKeys1D[i * 32 + j] = outputKeys1D[(i-1) * 32 + j] ^ lastColumn[j];
            outputKeys1D[i * 32 + j + 4] = outputKeys1D[(i-1) * 32 + j + 4] ^ outputKeys1D[i * 32 + j];
            outputKeys1D[i * 32 + j + 8] = outputKeys1D[(i-1) * 32 + j + 8] ^ outputKeys1D[i * 32 + j + 4];
            outputKeys1D[i * 32 + j + 12] = outputKeys1D[(i-1) * 32 + j + 12] ^ outputKeys1D[i * 32 + j + 8];
            if(i == 7) //Cut the last 16 bytes because 15*16 is shorter than  8*32
                continue;
            outputKeys1D[i * 32 + j + 16] = outputKeys1D[(i-1) * 32 + j + 16] ^ S_Rijndael[outputKeys1D[i * 32 + j + 12]];
            outputKeys1D[i * 32 + j + 20] = outputKeys1D[(i-1) * 32 + j + 20] ^ outputKeys1D[i * 32 + j + 16];
            outputKeys1D[i * 32 + j + 24] = outputKeys1D[(i-1) * 32 + j + 24] ^ outputKeys1D[i * 32 + j + 20];
            outputKeys1D[i * 32 + j + 28] = outputKeys1D[(i-1) * 32 + j + 28] ^ outputKeys1D[i * 32 + j + 24];
        }
    }
}

static void encrypt_block(uint8_t block[16], uint8_t keys[15][16]){
    //Initial Round
    for(unsigned int i = 0; i < 16; i++){
        block[i] ^= keys[0][i]; 
    }

    for(unsigned int i = 1; i < 14; i++){
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
    for(unsigned int j = 0; j < 16; j++){
        block[j] = S_Rijndael[block[j]];
    }

    shift_rows(block);

    for(unsigned int j = 0; j < 16; j++){
        block[j] ^= keys[14][j];
    }
}


//Return size encryped in dest
size_t aes256_encrypt_ECB(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[32]){
    uint8_t keys[15][16];
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

        memcpy(&(((uint8_t *) dest)[encrypted]), block, 16);
        encrypted += 16;
    }
    while (dataLeft);

    return encrypted;
}

size_t aes256_encrypt_CBC(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[16]){
    uint8_t keys[15][16];
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
        memcpy(&(((uint8_t *) dest)[encrypted]), block, 16);
        encrypted += 16;
    }
    while (dataLeft);

    return encrypted;
}

size_t aes256_encrypt_PCBC(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[16]){
    uint8_t keys[15][16];
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

        memcpy(&(((uint8_t *) dest)[encrypted]), block, 16);
        encrypted += 16;
    }
    while (dataLeft);

    return encrypted;
}

size_t aes256_encrypt_CTR(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[8]){
    uint8_t keys[15][16];
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

        memcpy(&(((uint8_t *) dest)[encrypted]), block, 16);
        encrypted += 16;
        counter++;
    }
    while (dataLeft);

    return encrypted;
}

size_t aes256_encrypt_CFB(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[16]){
    uint8_t keys[15][16];
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

        memcpy(&(((uint8_t *) dest)[encrypted]), lastBlock, 16);
        encrypted += 16;
    }
    while (dataLeft);

    return encrypted;
}

static void decrypt_block(uint8_t block[16], uint8_t keys[15][16]){
    for(unsigned int i = 0; i < 16; i++){
        block[i] ^= keys[14][i];
    }

    undo_shift_rows(block);

    for(unsigned int i = 0; i < 16; i++){
        block[i] = S_RijndaelInverse[block[i]];
    }


    for(unsigned int i = 13; i > 0; i--){
        for(unsigned int j = 0; j < 16; j++){
            block[j] ^= keys[i][j];
        }

        undo_mix_columns(block);

        undo_shift_rows(block);

        for(unsigned int i = 0; i < 16; i++){
            block[i] = S_RijndaelInverse[block[i]];
        }
    }

    for(unsigned int i = 0; i < 16; i++){
        block[i] ^= keys[0][i];
    }
}

void aes256_decrypt_ECB(const void *input, void *dest, size_t inputSize, uint8_t key[32]){
    uint8_t keys[15][16];
    key_schedule(key, keys);

    uint8_t block[16];
    while(inputSize != 0){
        memcpy(block, input, 16);

        decrypt_block(block, keys);

        memcpy(dest, block, 16);
        inputSize -= 16;
        input += 16;
        dest += 16;
    }
}

void aes256_decrypt_CBC(const void *input, void *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[16]){
    uint8_t keys[15][16];
    key_schedule(key, keys);

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    while(inputSize != 0){
        memcpy(block, input, 16);

        decrypt_block(block, keys);

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= lastBlock[i];
        }

        memcpy(dest, block, 16);
        memcpy(lastBlock, input, 16);
        inputSize -= 16;
        input += 16;
        dest += 16;
    }
}

void aes256_decrypt_PCBC(const void *input, void *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[16]){
    uint8_t keys[15][16];
    key_schedule(key, keys);

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    while(inputSize != 0){
        memcpy(block, input, 16);

        decrypt_block(block, keys);

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= lastBlock[i];
        }

        memcpy(lastBlock, input, 16);
        for(unsigned int i = 0; i < 16; i++){
            lastBlock[i] ^= block[i];
        }

        memcpy(dest, block, 16);
        inputSize -= 16;
        input += 16;
        dest += 16;
    }
}

void aes256_decrypt_CTR(const void *input, void *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[8]){
    uint8_t keys[15][16];
    key_schedule(key, keys);


    uint64_t counter = 0;
    uint8_t counterBlock[16];
    uint8_t block[16];
    while(inputSize != 0){
        memcpy(block, input, 16);

        memcpy(counterBlock, initVector, 8);
        memcpy(counterBlock + 8, &counter, 8);
        encrypt_block(counterBlock, keys);   

        for(unsigned int i = 0; i < 16; i++){
            block[i] ^= counterBlock[i];
        }

        memcpy(dest, block, 16);
        inputSize -= 16;
        input += 16;
        dest += 16;
        counter++;
    }
}

void aes256_decrypt_CFB(const void *input, void *dest, size_t inputSize, uint8_t key[32], const uint8_t initVector[16]){
    uint8_t keys[15][16];
    key_schedule(key, keys);

    uint8_t lastBlock[16];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    uint8_t block[16];
    while(inputSize != 0){
        memcpy(block, input, 16);

        encrypt_block(lastBlock, keys);

        for(unsigned int i = 0; i < 16; i++){
            lastBlock[i] ^= block[i];
        }

        memcpy(dest, lastBlock, 16);
        memcpy(lastBlock, block, 16);
        inputSize -= 16;
        input += 16;
        dest += 16;
    }
}