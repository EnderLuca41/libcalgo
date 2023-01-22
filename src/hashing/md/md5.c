#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <util.h>

#include <stdio.h>

const uint32_t s[64] = { 
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 
};


const uint32_t K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 
};

typedef struct {
    const uint8_t* input;
    uint64_t inputLen;
    uint64_t currentLen;
    bool done;
    bool single_one; //If single one is already appended
} Md5Context;

//Generates a 512 bit large chunk for the md5 algorithm
static bool generate_chunk(uint8_t chunk[64], Md5Context *context){
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
    * */
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

void md5(uint8_t hash[16], const void *input, size_t len){

    //Start values for Md5
    uint32_t a0 = 0x67452301;   
    uint32_t b0 = 0xefcdab89;   
    uint32_t c0 = 0x98badcfe;   
    uint32_t d0 = 0x10325476; 

    Md5Context context;
    context.currentLen = len;
    context.inputLen = len;
    context.input = input;
    context.done = false;
    context.single_one = false;

    uint8_t chunk[64];

    while(generate_chunk(chunk, &context)){
        //The 512 bit Chunk but as uin32_t's
        uint32_t M[16];
        memcpy(M, chunk, sizeof chunk);

        /*
        * Faster but insecure way
        * uint32_t *M = &chunk[0];
        */
        uint32_t A = a0;
        uint32_t B = b0;
        uint32_t C = c0;
        uint32_t D = d0;

        for(int i = 0; i < 64; i++){
            uint32_t F,g;
            if(i >= 0 && i <= 15){
                F = (B & C) | (~B & D);
                g = i;
            }
            if(i >= 16 && i <= 31){
                F = (D & B) | (~D & C);
                g = (5 * i + 1) % 16;
            }
            if(i >= 32 && i <= 47){
                F = B ^ C ^ D;
                g = (3 * i + 5) % 16;
            }
            if(i >= 48 && i <= 63){
                F = C ^ (B | ~D);
                g = (7 * i) % 16;
            }

            F = F + A + K[i] + M[g];
            A = D;
            D = C;
            C = B;
            B = B + left_rot32(F, s[i]);
        }

        a0 = a0 + A;
        b0 = b0 + B;
        c0 = c0 + C;
        d0 = d0 + D; 
    }

    //Copying of the hash, we do it with memcpy because we assume that are target mashine is little-endian
    memcpy(&hash[0], &a0, 4);
    memcpy(&hash[4], &b0, 4);
    memcpy(&hash[8], &c0, 4);
    memcpy(&hash[12], &d0, 4);
}

    