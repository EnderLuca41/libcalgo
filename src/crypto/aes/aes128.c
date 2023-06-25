#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "aesutil.h"



static void key_schedule(uint8_t initKey[16], uint8_t outputKeys[11][16]){
    memcpy(outputKeys[0], initKey, 16);

    for(unsigned int i = 1; i < 11; i++){
        uint8_t lastColumn[4];
        for(unsigned j = 0; j < 4; j++){
            lastColumn[j] = outputKeys[i-1][j + 12];
        }

        rotword(lastColumn);
        subword(lastColumn);
        lastColumn[0] ^= Rc[i];

        for(unsigned int j = 0; j < 4; j++){
            outputKeys[i][j] = outputKeys[i-1][j] ^ lastColumn[j];
            outputKeys[i][j + 4] = outputKeys[i-1][j + 4] ^ outputKeys[i][j]; 
            outputKeys[i][j + 8] = outputKeys[i-1][j + 8] ^ outputKeys[i][j + 4]; 
            outputKeys[i][j + 12] = outputKeys[i-1][j + 12] ^ outputKeys[i][j + 8]; 
        }
    }
}

static void encrypt_block(uint8_t block[16], uint8_t keys[11][16]){
    //Initial Round
    for(unsigned int i = 0; i < 16; i++){
        block[i] ^= keys[0][i]; 
    }

    for(unsigned int i = 1; i < 10; i++){
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
        block[j] ^= keys[10][j];
    }
}

//Return size encryped in dest
size_t aes128_encrypt_ECB(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[16]){
    uint8_t keys[11][16];
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

size_t aes128_encrypt_CBC(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[16], const uint8_t initVector[16]){
    uint8_t keys[11][16];
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

size_t aes128_encrypt_PCBC(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[16], const uint8_t initVector[16]){
    uint8_t keys[11][16];
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
        
        encrypt_bloqck(block, keys);        

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

static void decrypt_block(uint8_t block[16], uint8_t keys[11][16]){
    for(unsigned int i = 0; i < 16; i++){
        block[i] ^= keys[10][i];
    }

    undo_shift_rows(block);

    for(unsigned int i = 0; i < 16; i++){
        block[i] = S_RijndaelInverse[block[i]];
    }


    for(unsigned int i = 9; i > 0; i--){
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

void aes128_decrypt_ECB(const void *input, void *dest, size_t inputSize, uint8_t key[16]){
    uint8_t keys[11][16];
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

void aes128_decrypt_CBC(const void *input, void *dest, size_t inputSize, uint8_t key[16], const uint8_t initVector[16]){
    uint8_t keys[11][16];
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

void aes128_decrypt_PCBC(const void *input, void *dest, size_t inputSize, uint8_t key[16], const uint8_t initVector[16]){
    uint8_t keys[11][16];
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