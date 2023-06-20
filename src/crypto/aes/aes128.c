#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <util.h>
#include "aesutil.h"


static int mod(int x, int N){
    return (x % N + N) %N;
}

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


void shift_rows(uint8_t b[16]){
    for(unsigned int i = 1; i < 4; i++){
        uint32_t buffer = 0;
        uint8_t shifts = 24;

        for(unsigned int j = 0; j < 16; j+=4){
            buffer |= ((uint32_t) b[j + i]) << shifts;
            shifts -= 8;
        }

        buffer = left_rot32(buffer, i * 8);

        shifts = 24;
        for(unsigned int j = 0; j < 16; j+=4){
            b[j + i] = (buffer >> shifts) & 0xff;
            shifts -= 8;
        }
    }
}

static void mix_columns(uint8_t b[16]){
     for(unsigned int j = 0; j < 4; j++){
        uint8_t column[4];
        for(unsigned int k = 0; k < 4; k++) {
            column[k] = b[j * 4 + k];
        }

        mix_column(column);

        for(unsigned int k = 0; k < 4; k++){
            b[j * 4 + k] = column[k];
        }
    }
}



typedef struct {
    const uint8_t *input;
    size_t inputSize;
}    
AesBuffer;

static bool generate_block(uint8_t block[16], AesBuffer *buffer){
    if(buffer->inputSize <= 16){
        memcpy(block, buffer->input, buffer->inputSize);
        memset(((void*) block) + buffer->inputSize, 0, 16 - buffer->inputSize);
        buffer->input -= buffer->inputSize;
        buffer->inputSize = 0;
        return false;
    }

    memcpy(block, buffer->input, 16);
    buffer->input += 16;
    buffer->inputSize -= 16;

    return true;
}

//Return size encryped in dest
size_t aes128_encrypt(const void *input, uint8_t *dest, size_t inputSize, uint8_t key[16]){
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
        

        memcpy(&(((uint8_t *) dest)[encrypted]), block, 16);
        encrypted += 16;
    }
    while (dataLeft);

    return encrypted;
}


static void undo_shift_rows(uint8_t b[16]){
    for(unsigned int i = 1; i < 4; i++){
        uint32_t buffer = 0;
        uint8_t shifts = 24;

        for(unsigned int j = 0; j < 16; j+=4){
            buffer |= ((uint32_t) b[j + i]) << shifts;
            shifts -= 8;
        }

        buffer = right_rot32(buffer, i * 8);

        shifts = 24;
        for(unsigned int j = 0; j < 16; j+=4){
            b[j + i] = (buffer >> shifts) & 0xff;
            shifts -= 8;
        }
    }
}

static void undo_mix_columns(uint8_t b[16]){
     for(unsigned int j = 0; j < 4; j++){
        uint8_t column[4];
        for(unsigned int k = 0; k < 4; k++) {
            column[k] = b[j * 4 + k];
        }

        undo_mix_column(column);

        for(unsigned int k = 0; k < 4; k++){
            b[j * 4 + k] = column[k];
        }
    }
}

void aes128_decrypt(const void *input, void *dest, size_t inputSize, uint8_t key[16]){
    uint8_t keys[11][16];
    key_schedule(key, keys);

    uint8_t block[16];
    while(true){
        memcpy(block, input, 16);

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

        memcpy(dest, block, 16);
        inputSize -= 16;
        input += 16;
        dest += 16;
        if(inputSize == 0)
            break;
    }
}
