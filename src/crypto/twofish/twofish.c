#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "twofishutil.h"
#include <util.h>

static void encrypt_block(uint32_t block[4], const TwofishContext *ctx){

    //Input whitening
    block[0] ^= ctx->subkeys[0];
    block[1] ^= ctx->subkeys[1];
    block[2] ^= ctx->subkeys[2];
    block[3] ^= ctx->subkeys[3];



    for(unsigned int i = 0; i < 16; i++){
        uint32_t block0 = block[0];
        uint32_t block1 = block[1];

        //Key dependent S-Boxes
        uint32_t buff = ctx->s0[block[0] & 0xff];
        buff ^= ctx->s1[(block[0] >> 8) & 0xff];
        buff ^= ctx->s2[(block[0] >> 16) & 0xff];
        buff ^= ctx->s3[(block[0] >> 24) & 0xff];

        block[0] = buff;
    
        block[1] = left_rot32(block[1], 8);
        buff = ctx->s0[block[1] & 0xff];
        buff ^= ctx->s1[(block[1] >> 8) & 0xff];
        buff ^= ctx->s2[(block[1] >> 16) & 0xff];
        buff ^= ctx->s3[(block[1] >> 24) & 0xff];

        block[1] = buff;

        //PHT
        block[0] += block[1];
        block[1] += block[0];

        block[0] += ctx->subkeys[2 * i + 8];
        block[1] += ctx->subkeys[2 * i + 9];
        
        block[2] ^= block[0];
        block[2] = right_rot32(block[2], 1);

        block[3] = left_rot32(block[3], 1);
        block[3] ^= block[1];

        //Swap
        block[0] = block[2];
        block[1] = block[3];
        block[2] = block0;
        block[3] = block1;
    }

    //Undo last swap
    uint32_t block0 = block[0];
    uint32_t block1 = block[1];
    block[0] = block[2];
    block[1] = block[3];
    block[2] = block0;
    block[3] = block1;

    //Output Wwitening
    block[0] ^= ctx->subkeys[4];
    block[1] ^= ctx->subkeys[5];
    block[2] ^= ctx->subkeys[6];
    block[3] ^= ctx->subkeys[7];
}

//Returns size of encrypted bytes in dest
static size_t twofish_encrypt_ECB(const void *input, void *dest, size_t inputLen, const uint8_t *key, uint8_t keyLen){    
    //Calculate the subkeys and the key dependent S-Boxes
    TwofishContext ctx;
    twofish_init(key, keyLen, &ctx);

    size_t size = 0;
    uint32_t block[4];
    while(inputLen != 0){
        if(inputLen >= 16)
            memcpy(block, &(((uint8_t *) input)[size]), sizeof(block));
        else {
            memcpy(block, &(((uint8_t *) input)[size]), inputLen);
            memset(((void*) block) + inputLen, 0, 16 - inputLen); //Pad with Zeros
        }

        encrypt_block(block, &ctx);
        memcpy(dest, block, sizeof(block));

        dest += 16;
        inputLen -= 16;
        size += 16;
    }

    return true;
}

size_t twofish128_encrypt_ECB(const void *input, void *dest, size_t inputLen, const uint8_t key[16]){
    return twofish_encrypt_ECB(input, dest, inputLen, key, 16);
}

size_t twofish192_encrypt_ECB(const void *input, void *dest, size_t inputLen, const uint8_t key[24]){
    return twofish_encrypt_ECB(input, dest, inputLen, key, 24);
}

size_t twofish256_encrypt_ECB(const void *input, void *dest, size_t inputLen, const uint8_t key[32]){
    return twofish_encrypt_ECB(input, dest, inputLen, key, 32);
}

//Returns size of encrypted bytes in dest
static size_t twofish_encrypt_CBC(const void *input, void *dest, size_t inputLen, const uint8_t *key, uint8_t keyLen, const uint8_t initVector[16]){    
    //Calculate the subkeys and the key dependent S-Boxes
    TwofishContext ctx;
    twofish_init(key, keyLen, &ctx);

    uint32_t lastBlock[4];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    size_t size = 0;
    uint32_t block[4];
    while(inputLen != 0){
        if(inputLen >= 16)
            memcpy(block, &(((uint8_t *) input)[size]), sizeof(block));
        else {
            memcpy(block, &(((uint8_t *) input)[size]), inputLen);
            memset(((void*) block) + inputLen, 0, 16 - inputLen); //Pad with Zeros
        }

        for(unsigned int i = 0; i < 4; i++){
            block[i] ^= lastBlock[i];
        }

        encrypt_block(block, &ctx);

        memcpy(dest, block, sizeof(block));
        memcpy(lastBlock, block, sizeof(lastBlock));

        dest += 16;
        inputLen -= 16;
        size += 16;
    }

    return true;
}

size_t twofish128_encrypt_CBC(const void *input, void *dest, size_t inputLen, const uint8_t key[16], const uint8_t initVector[16]){
    return twofish_encrypt_CBC(input, dest, inputLen, key, 16, initVector);
}

size_t twofish192_encrypt_CBC(const void *input, void *dest, size_t inputLen, const uint8_t key[24], const uint8_t initVector[16]){
    return twofish_encrypt_CBC(input, dest, inputLen, key, 24, initVector);
}

size_t twofish256_encrypt_CBC(const void *input, void *dest, size_t inputLen, const uint8_t key[32], const uint8_t initVector[16]){
    return twofish_encrypt_CBC(input, dest, inputLen, key, 32, initVector);
}

static void decrypt_block(uint32_t block[4], const TwofishContext *ctx){
    block[0] ^= ctx->subkeys[4];
    block[1] ^= ctx->subkeys[5];
    block[2] ^= ctx->subkeys[6];
    block[3] ^= ctx->subkeys[7];

    for(int i = 15; i != -1; i--){


        uint32_t block0 = block[0];
        uint32_t block1 = block[1];

        //Key dependent S-Boxes
        uint32_t buff = ctx->s0[block[0] & 0xff];
        buff ^= ctx->s1[(block[0] >> 8) & 0xff];
        buff ^= ctx->s2[(block[0] >> 16) & 0xff];
        buff ^= ctx->s3[(block[0] >> 24) & 0xff];

        block[0] = buff;
    
        block[1] = left_rot32(block[1], 8);
        buff = ctx->s0[block[1] & 0xff];
        buff ^= ctx->s1[(block[1] >> 8) & 0xff];
        buff ^= ctx->s2[(block[1] >> 16) & 0xff];
        buff ^= ctx->s3[(block[1] >> 24) & 0xff];

        block[1] = buff;

        //PHT
        block[0] += block[1];
        block[1] += block[0];

        block[0] += ctx->subkeys[2 * i + 8];
        block[1] += ctx->subkeys[2 * i + 9];
        
        block[2] = left_rot32(block[2], 1);
        block[2] ^= block[0];

        block[3] ^= block[1];
        block[3] = right_rot32(block[3], 1);

        //Swap
        block[0] = block[2];
        block[1] = block[3];
        block[2] = block0;
        block[3] = block1;
    }

    uint32_t block0 = block[0];
    uint32_t block1 = block[1];
    block[0] = block[2];
    block[1] = block[3];
    block[2] = block0;
    block[3] = block1;

    block[0] ^= ctx->subkeys[0];
    block[1] ^= ctx->subkeys[1];
    block[2] ^= ctx->subkeys[2];
    block[3] ^= ctx->subkeys[3];
}

//Returns true if decryption was successful,
//Returns false if inputSize is not 0 when mod 16
static bool twofish_decrypt_ECB(const void *input, void *dest, size_t inputSize, const uint8_t *key, uint8_t keyLen){
    if(inputSize % 16 != 0)
        return false;

    TwofishContext ctx;
    twofish_init(key, keyLen, &ctx);
    uint32_t block[4];
    while(inputSize != 0){
        memcpy(block, input, sizeof(block));

        decrypt_block(block, &ctx);

        memcpy(dest, block, sizeof(block));

        inputSize -= 16;
        input += 16;
        dest += 16;
    }

    return true;
}

bool twofish128_decrypt_ECB(const void *input, void *dest, size_t inputSize, const uint8_t *key){
    return twofish_decrypt_ECB(input, dest, inputSize, key, 16);
}

bool twofish192_decrypt_ECB(const void *input, void *dest, size_t inputSize, const uint8_t *key){
    return twofish_decrypt_ECB(input, dest, inputSize, key, 24);
}

bool twofish256_decrypt_ECB(const void *input, void *dest, size_t inputSize, const uint8_t *key){
    return twofish_decrypt_ECB(input, dest, inputSize, key, 32);
}

//Returns true if decryption was successful,
//Returns false if inputSize is not 0 when mod 16
static bool twofish_decrypt_CBC(const void *input, void *dest, size_t inputSize, const uint8_t *key, uint8_t keyLen, const uint8_t initVector[16]){
    if(inputSize % 16 != 0)
        return false;

    uint32_t lastBlock[4];
    memcpy(lastBlock, initVector, sizeof(lastBlock));

    TwofishContext ctx;
    twofish_init(key, keyLen, &ctx);
    uint32_t block[4];
    while(inputSize != 0){
        memcpy(block, input, sizeof(block));

        decrypt_block(block, &ctx);

        for(unsigned int i = 0; i < 4; i++){
            block[i] ^= lastBlock[i];
        }

        memcpy(dest, block, sizeof(block));
        memcpy(lastBlock, input, sizeof(lastBlock));

        inputSize -= 16;
        input += 16;
        dest += 16;
    }

    return true;
}

bool twofish128_decrypt_CBC(const void *input, void *dest, size_t inputSize, const uint8_t *key, const uint8_t initVector[16]){
    return twofish_decrypt_CBC(input, dest, inputSize, key, 16, initVector);
}

bool twofish192_decrypt_CBC(const void *input, void *dest, size_t inputSize, const uint8_t *key, const uint8_t initVector[16]){
    return twofish_decrypt_CBC(input, dest, inputSize, key, 24, initVector);
}

bool twofish256_decrypt_CBC(const void *input, void *dest, size_t inputSize, const uint8_t *key, const uint8_t initVector[16]){
    return twofish_decrypt_CBC(input, dest, inputSize, key, 32, initVector);
}
