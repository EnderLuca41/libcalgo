#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static uint64_t left_rot28(uint64_t x, uint8_t n){ return ((x << n) & 0xFFFFFFF) | (x >> (28-n)); }

//Lookup Tables
const static uint8_t IP_Table[64] = {
    58, 50, 42, 34, 26,	18,	10, 2,
    60,	52,	44, 36,	28,	20,	12,	4,
    62,	54,	46,	38,	30,	22,	14,	6,
    64,	56,	48,	40,	32,	24,	16,	8,
    57,	49,	41,	33,	25,	17,	9,	1,
    59,	51,	43,	35,	27,	19,	11,	3,
    61,	53,	45,	37,	29,	21,	13,	5,
    63,	55,	47,	39,	31,	23,	15,	7
};

const static uint8_t FP_Table[64] = {
    40,	8,  48, 16, 56,	24,	64,	32,
    39,	7,  47, 15,	55,	23,	63, 31,
    38,	6,	46,	14,	54,	22,	62,	30,
    37,	5,	45,	13,	53,	21,	61,	29,
    36,	4,	44,	12,	52,	20,	60,	28,
    35,	3,	43,	11,	51,	19,	59,	27,
    34,	2,	42,	10,	50,	18,	58,	26,
    33,	1,	41,	9,	49,	17,	57,	25 
};

const static uint8_t PC1_Table[56] = {
    57,	49,	41,	33,	25, 17,	9,
    1,	58,	50,	42,	34,	26,	18,
    10,	2,	59,	51,	43,	35,	27,
    19,	11,	3,	60,	52,	44,	36,
    63,	55,	47,	39,	31,	23,	15,
    7,	62,	54,	46,	38,	30,	22,
    14,	6,	61,	53,	45,	37,	29,
    21,	13,	5,	28,	20,	12,	4
};

const static uint8_t PC2_Table[48] = {
    14,	17, 11, 24,	1,	5,
    3,	28,	15,	6,	21,	10,
    23,	19,	12,	4,	26,	8,
    16,	7,	27,	20,	13,	2,
    41,	52,	31,	37,	47,	55,
    30,	40, 51,	45,	33,	48,
    44, 49,	39,	56,	34,	53,
    46,	42,	50,	36,	29,	32 
};

const static uint8_t E_Table[] = {
    32,	1,	2,	3,	4,	5,
    4,	5,	6,	7,	8,	9,
    8,	9,	10,	11,	12,	13,
    12,	13,	14,	15,	16,	17,
    16,	17,	18,	19,	20,	21,
    20,	21,	22,	23,	24,	25,
    24,	25,	26,	27,	28,	29,
    28,	29,	30,	31,	32,	1 
};

const static uint8_t P_Table[32] = {
    16,	7,	20,	21,	29,	12,	28,	17,
    1,	15,	23,	26,	5,	18,	31,	10,
    2,	8,	24,	14,	32,	27, 3,	9,
    19,	13,	30,	6,	22,	11,	4,	25 
};

const static uint8_t Rotations[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 
};

const static uint8_t S1[64] = {
    14, 0, 4, 15, 13, 7, 1, 4,
    2, 14, 15, 2, 11, 13, 8, 1,
    3, 10, 10, 6, 6, 12, 12, 11, 
    5, 9, 9, 5, 0, 3, 7, 8, 
    4, 15, 1, 12, 14, 8, 8, 2, 
    13, 4, 6, 9, 2, 1, 11, 7, 
    15, 5, 12, 11, 9, 3, 7, 14, 
    3, 10, 10, 0, 5, 6, 0, 13
};

const static uint8_t S2[64] = {
    15, 3, 1, 13, 8, 4, 14, 7,
    6, 15, 11, 2, 3, 8, 4, 14,
    9, 12, 7, 0, 2, 1, 13, 10,
    12, 6, 0, 9, 5, 11, 10, 5, 
    0, 13, 14, 8, 7, 10, 11, 1, 
    10, 3, 4, 15, 13, 4, 1, 2, 
    5, 11, 8, 6, 12, 7, 6, 12, 
    9, 0, 3, 5, 2, 14, 15, 9
};

const static uint8_t S3[64] = {
    10, 13, 0, 7, 9, 0, 14, 9, 
    6, 3, 3, 4, 15, 6, 5, 10, 
    1, 2, 13, 8, 12, 5, 7, 14, 
    11, 12, 4, 11, 2, 15, 8, 1, 
    13, 1, 6, 10, 4, 13, 9, 0, 
    8, 6, 15, 9, 3, 8, 0, 7, 
    11, 4, 1, 15, 2, 14, 12, 3, 
    5, 11, 10, 5, 14, 2, 7, 12
};

const static uint8_t S4[] = {
    7, 13, 13, 8, 14, 11, 3, 5, 
    0, 6, 6, 15, 9, 0, 10, 3, 
    1, 4, 2, 7, 8, 2, 5, 12, 
    11, 1, 12, 10, 4, 14, 15, 9, 
    10, 3, 6, 15, 9, 0, 0, 6, 
    12, 10, 11, 1, 7, 13, 13, 8, 
    15, 9, 1, 4, 3, 5, 14, 11, 
    5, 12, 2, 7, 8, 2, 4, 14
};

const static uint8_t S5[64] = {
    2, 14, 12, 11, 4, 2, 1, 12, 
    7, 4, 10, 7, 11, 13, 6, 1, 
    8, 5, 5, 0, 3, 15, 15, 10, 
    13, 3, 0, 9, 14, 8, 9, 6, 
    4, 11, 2, 8, 1, 12, 11, 7, 
    10, 1, 13, 14, 7, 2, 8, 13, 
    15, 6, 9, 15, 12, 0, 5, 9, 
    6, 10, 3, 4, 0, 5, 14, 3, 
};

const static uint8_t S6[64] = {
    12, 10, 1, 15, 10, 4, 15, 2, 
    9, 7, 2, 12, 6, 9, 8, 5, 
    0, 6, 13, 1, 3, 13, 4, 14, 
    14, 0, 7, 11, 5, 3, 11, 8, 
    9, 4, 14, 3, 15, 2, 5, 12, 
    2, 9, 8, 5, 12, 15, 3, 10, 
    7, 11, 0, 14, 4, 1, 10, 7, 
    1, 6, 13, 0, 11, 8, 6, 13
};

const static uint8_t S7[64] = {
    4, 13, 11, 0, 2, 11, 14, 7, 
    15, 4, 0, 9, 8, 1, 13, 10, 
    3, 14, 12, 3, 9, 5, 7, 12, 
    5, 2, 10, 15, 6, 8, 1, 6, 
    1, 6, 4, 11, 11, 13, 13, 8, 
    12, 1, 3, 4, 7, 10, 14, 7, 
    10, 9, 15, 5, 6, 0, 8, 15, 
    0, 14, 5, 2, 9, 3, 2, 12
};

const static uint8_t S8[64] = {
    13, 1, 2, 15, 8, 13, 4, 8, 
    6, 10, 15, 3, 11, 7, 1, 4, 
    10, 12, 9, 5, 3, 6, 14, 11, 
    5, 0, 0, 14, 12, 9, 7, 2, 
    7, 2, 11, 1, 4, 14, 1, 7, 
    9, 4, 12, 10, 14, 8, 2, 13, 
    0, 15, 6, 12, 10, 9, 13, 0, 
    15, 3, 3, 5, 5, 6, 8, 11, 
};

static uint64_t IP(uint64_t val){
    uint64_t ret = 0;
	for(int i = 0; i < 64; i++){
        ret ^= ((val >> (64 - IP_Table[i])) & 0b01) << (63-i);
    }
    return ret;
}

static uint64_t FP(uint64_t val){
    uint64_t ret = 0;
	for(int i = 0; i < 64; i++){
        ret ^= ((val >> (64 - FP_Table[i])) & 0b01) << (63-i);
    }
    return ret;
}

static uint64_t PC1(uint64_t val){
    uint64_t ret = 0;
    for(int i = 0; i < 56; i++){
        ret ^= ((val >> (64 - PC1_Table[i])) & 0b01) << (55-i);
    }
    return ret;
}

static uint64_t PC2(uint64_t val){
    uint64_t ret = 0;
    for(int i = 0; i < 48; i++){
        ret ^= ((val >> (56 - PC2_Table[i])) & 0b01) << (47-i);
    }
    return ret;
}

static uint64_t E(uint32_t val){
    uint64_t val64 = val;
    uint64_t ret = 0;
    for(int i = 0; i < 48; i++){
        ret ^= ((val64 >> (32 - E_Table[i])) & 0b01) << (47-i);
    }
    return ret;
}

static uint32_t P(uint32_t val){
    uint32_t ret = 0;
    for(int i = 0; i < 32; i++){
        ret ^= ((val >> (32 - P_Table[i])) & 0b01) << (31-i);
    }
    return ret;
}

static uint32_t F(uint32_t val, uint64_t key){
    uint64_t val64 = E(val);
    val64 ^= key;
    val = 0;

    val ^= S1[(val64 >> 42) & 0b111111];
    val <<= 4;
    val ^= S2[(val64 >> 36) & 0b111111];
    val <<= 4;
    val ^= S3[(val64 >> 30) & 0b111111];
    val <<= 4;
    val ^= S4[(val64 >> 24) & 0b111111];
    val <<= 4;
    val ^= S5[(val64 >> 18) & 0b111111];
    val <<= 4;
    val ^= S6[(val64 >> 12) & 0b111111];
    val <<= 4;
    val ^= S7[(val64 >> 6) & 0b111111];
    val <<= 4;
    val ^= S8[val64 & 0b111111];


    return P(val);
}

void des_encrypt(uint8_t *data, uint8_t *dest, size_t dataLen, uint64_t key){
    key = PC1(key);
    uint64_t left = (key >> 28) & 0xFFFFFFF;
    uint64_t right = key & 0xFFFFFFF;


    //Key schedule
    static uint64_t subkeys[16];
    for(uint32_t i = 0; i < 16; i++){
        left = left_rot28(left, Rotations[i]);
        right = left_rot28(right, Rotations[i]);
        uint64_t concat = right | (left << 28);
        subkeys[i] = PC2(concat);
    }

    uint64_t block = 0;
    bool dataLeft = true;
    while(dataLeft){
        if(dataLen <= 8){
            dataLeft = false;
            block = 0;
            memcpy(&block, data, dataLen);
            data += dataLen;
        }
        else {
            memcpy(&block, data, 8);
            dataLen -= 8;
            data += 8;
        }


        block = IP(block);
        uint32_t left = block >> 32;
        uint32_t right = block;

        //16 Feistel Rounds
        for(int i = 0; i < 16; i++){
            uint32_t tmp = right;
            right = F(right, subkeys[i]);
            right = right ^ left;
            left = tmp;
        }


        //Swap
        uint32_t tmp = right;
        right = left;
        left = tmp;

        //Finial Permutation
        block = (((uint64_t) left) << 32) ^ right;
        block = FP(block);
        memcpy(dest, &block, sizeof block);
        dest += 8;
    }

}

void des_decrypt(uint8_t *data, uint8_t *dest, size_t dataLen, uint64_t key){
    key = PC1(key);
    uint64_t left = (key >> 28) & 0xFFFFFFF;
    uint64_t right = key & 0xFFFFFFF;


    //Key schedule
    uint64_t subkeys[16];
    for(uint32_t i = 0; i < 16; i++){
        left = left_rot28(left, Rotations[i]);
        right = left_rot28(right, Rotations[i]);
        uint64_t concat = right | (left << 28);
        subkeys[i] = PC2(concat);
    }

    uint64_t block;
    bool dataLeft = true;
    while(dataLeft){
        memcpy(&block, data, 8);
        dataLen -= 8;
        data += 8;
        
        if(dataLen == 0)
            dataLeft = false;

        block = IP(block);
        uint32_t left = block >> 32;
        uint32_t right = block;

        uint64_t k = (((uint64_t) left) << 32) ^ right;

        //16 Feistel Rounds
        for(int i = 15; i > -1; i--){
            uint32_t tmp = right;
            right = F(right, subkeys[i]);
            right = right ^ left;
            left = tmp;
        }

        //Swap
        uint32_t tmp = right;
        right = left;
        left = tmp;

        //Final Permutation
        block = (((uint64_t) left) << 32) ^ right;
        block = FP(block);
        memcpy(dest, &block, sizeof block);
        dest += 8;
    }
}
