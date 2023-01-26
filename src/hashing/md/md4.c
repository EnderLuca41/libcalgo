#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <util.h>

//S Constants for md4
#define S11 3
#define S12 7
#define S13 11
#define S14 19
#define S21 3
#define S22 5
#define S23 9
#define S24 13
#define S31 3
#define S32 9
#define S33 11
#define S34 15

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & ((y) | (z))) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))

#define FF(a, b, c, d, x, s){ \
    (a) += F((b), (c), (d)) + (x); \
    (a) = left_rot32((a), (s)); \
}

#define GG(a, b, c, d, x, s){ \
    (a) += G((b), (c), (d)) + (x) + (uint32_t)0x5a827999; \
    (a) = left_rot32((a), (s)); \
}

#define HH(a, b, c, d, x, s){ \
    (a) += H((b), (c), (d)) + (x) + (uint32_t)0x6ed9eba1; \
    (a) = left_rot32((a), (s)); \
}


typedef struct {
    const uint8_t* input;
    uint64_t inputLen;
    uint64_t currentLen;
    bool done;
    bool single_one; //If single one is already appended
} Md4Context;

static bool generate_chunk(uint8_t chunk[64], Md4Context *context){
    if(context->done == true)
        return false;

    //When there is still 64 bytes or more of the input left,
    //They get simply copied into chunk
    if(context->currentLen >= 64){
        memcpy(chunk, context->input, 64);
        context->input += 64;
        context->currentLen -= 64;
        return true;        
    }

    //When there is 63 bytes of data or less left they get all copied into chunk
    memcpy(chunk, context->input, context->currentLen);
    size_t space_in_chunk = context->currentLen;
    chunk += space_in_chunk;
    context->input += context->currentLen;
    context->currentLen -= context->currentLen;

     //Append single one
    if(!context->single_one) {
        chunk[0] = 128;
        chunk++;
        space_in_chunk++;
        context->single_one = true;
    }
    /*
    * If there is not enough room in the chunk (8 bytes) for the length of the input,
    * The algoritm will pad the remaining space with zero-bytes
    */
    if(space_in_chunk > 56)
    {
        memset(chunk, '\0', 64 - space_in_chunk);
        return true;
    }

    /*
    * if the destionation has enough space in the chunk, all still free space except the last 8 bytes get assigned to zero-byte
    * The last 8 bytes then get filled with the length of the input in little Endian
    */
    memset(chunk, '\0', 64 - space_in_chunk - 8);
    chunk += 64 - space_in_chunk - 8;
    
    //Copy length in little endian
    uint64_t inputLen = context->inputLen * 8;
    memcpy(chunk, &inputLen, 8);
    
    context->done = true;
    return true;
}

void md4(uint8_t hash[16], const void *input, size_t len){
    uint32_t a0 = 0x67452301;   
    uint32_t b0 = 0xefcdab89;   
    uint32_t c0 = 0x98badcfe;   
    uint32_t d0 = 0x10325476; 

    Md4Context context;
    context.currentLen = len;
    context.inputLen = len;
    context.input = input;
    context.done = false;
    context.single_one = false;

    uint8_t chunk[64];
    while(generate_chunk(chunk, &context)){
        uint32_t M[16];
        memcpy(M, chunk, sizeof chunk);

        uint32_t A = a0;
        uint32_t B = b0;
        uint32_t C = c0;
        uint32_t D = d0;

        //First Round
        FF(A, B, C, D, M[0], S11);
        FF(D, A, B, C, M[1], S12);
        FF(C, D, A, B, M[2], S13);
        FF(B, C, D, A, M[3], S14);
        FF(A, B, C, D, M[4], S11);
        FF(D, A, B, C, M[5], S12);
        FF(C, D, A, B, M[6], S13);
        FF(B, C, D, A, M[7], S14);
        FF(A, B, C, D, M[8], S11);
        FF(D, A, B, C, M[9], S12);
        FF(C, D, A, B, M[10], S13);
        FF(B, C, D, A, M[11], S14);
        FF(A, B, C, D, M[12], S11);
        FF(D, A, B, C, M[13], S12);
        FF(C, D, A, B, M[14], S13);
        FF(B, C, D, A, M[15], S14);

        //Secound Round
        GG(A, B, C, D, M[0], S21);
        GG(D, A, B, C, M[4], S22);
        GG(C, D, A, B, M[8], S23);
        GG(B, C, D, A, M[12], S24);
        GG(A, B, C, D, M[1], S21);
        GG(D, A, B, C, M[5], S22);
        GG(C, D, A, B, M[9], S23);
        GG(B, C, D, A, M[13], S24);
        GG(A, B, C, D, M[2], S21);
        GG(D, A, B, C, M[6], S22);
        GG(C, D, A, B, M[10], S23);
        GG(B, C, D, A, M[14], S24);
        GG(A, B, C, D, M[3], S21);
        GG(D, A, B, C, M[7], S22);
        GG(C, D, A, B, M[11], S23);
        GG(B, C, D, A, M[15], S24);

        //Third Round
        HH(A, B, C, D, M[ 0], S31);
        HH(D, A, B, C, M[ 8], S32);
        HH(C, D, A, B, M[ 4], S33);
        HH(B, C, D, A, M[12], S34);
        HH(A, B, C, D, M[ 2], S31);
        HH(D, A, B, C, M[10], S32);
        HH(C, D, A, B, M[ 6], S33);
        HH(B, C, D, A, M[14], S34);
        HH(A, B, C, D, M[ 1], S31);
        HH(D, A, B, C, M[ 9], S32);
        HH(C, D, A, B, M[ 5], S33);
        HH(B, C, D, A, M[13], S34);
        HH(A, B, C, D, M[ 3], S31);
        HH(D, A, B, C, M[11], S32);
        HH(C, D, A, B, M[ 7], S33);
        HH(B, C, D, A, M[15], S34);

        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    
    //Copying of the hash, we do it with memcpy because we assume that are target mashine is little-endian
    memcpy(&hash[0], &a0, 4);
    memcpy(&hash[4], &b0, 4);
    memcpy(&hash[8], &c0, 4);
    memcpy(&hash[12], &d0, 4);
}