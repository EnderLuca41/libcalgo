#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <util.h>
#include <string.h>

extern const uint32_t k32[64];

typedef struct {
	FILE *input;
	uint64_t maxLen;
	uint64_t currentLen;
	bool single_one;
	bool done;
	bool allBytedsRead;
} Sha256Context;

static bool generate_chunk(uint8_t chunk[64], Sha256Context *context){
    if(context->done)
        return false;
    
    int space_in_chunk; //Already used space in current chunk
    //If there are 64 or more bytes to copy
    if(!context->allBytedsRead && context->currentLen + 64 <= context->maxLen){
        size_t lenRead = fread(chunk, sizeof(uint8_t), 64, context->input);
		context->currentLen += lenRead;

        if(lenRead == 64) //If there are still more bytes left
            return true;
        
        space_in_chunk = lenRead;
		context->allBytedsRead = true;
    }
    //If there are less then 64 bytes left to copy
    else if(!context->allBytedsRead)
    {
        size_t lenRead = fread(chunk, sizeof(uint8_t), context->maxLen % 64, context->input);
        space_in_chunk = lenRead;
        context->currentLen += lenRead;
		context->allBytedsRead = true;
    }

    chunk += space_in_chunk;

    //Append 0b10000000 byte
    if(!context->single_one){
        chunk[0] = 0x80;
        chunk++;
        space_in_chunk++;
        context->single_one = true;
    }

    //If there is not enough room in the chunk (8 bytes) for the length of the input,
    //The algoritm will pad the remaining space with zero-bytes
    if(space_in_chunk > 56){
        memset(chunk, 0, space_in_chunk - 64);
        return true;
    }

    /*
    * if the destionation has enough space in the chunk, all still free space except the last 8 bytes get assigned to zero-byte
    * The last 8 bytes then get filled with the length of the input in Big Endian
    */
    memset(chunk, '\0', 64 - space_in_chunk - 8);
    chunk += 64 - space_in_chunk - 8;
    
    //Copy length endian indpendent
	unsigned long len = context->currentLen * 8;
	uint8_t shift = 56;
	for(uint8_t i = 0; i < 8; i++){
		chunk[i] = (uint8_t) (len >> shift);
		shift -= 8;
	}
    
    context->done = true;
    return true;
}

//Sha256 but you can specify the h constants, useful for sha224 so you not have to rewrite everything
static void fsha224_256(uint8_t hash[32], FILE *input, size_t len, const uint32_t Hs[8]){
    Sha256Context context;
    context.currentLen = 0;
    context.done = false;
    context.input = input;
    context.maxLen = len;
    context.single_one = false;
    context.allBytedsRead = false;

    uint32_t hArr[8];
    memcpy(hArr, Hs, sizeof(uint32_t) * 8);
    
    uint8_t chunk[64];
    while (generate_chunk(chunk, &context)) {
        uint32_t w[64];
        uint32_t a,b,c,d,e,f,g,h;
        

        //copying the data of chunk in the first 16 elements and fill the rest with 0
        const uint8_t *p = chunk;
        memset(w, 0x00, sizeof w);
        for (int i = 0; i < 16; i++) {
            w[i] = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 |
                (uint32_t) p[2] << 8 | (uint32_t) p[3];
            p += 4;
        }

        for(int i = 16; i < 64; i++){
            const uint32_t s0 = right_rot32(w[i - 15], 7) ^ right_rot32(w[i - 15], 18) ^ (w[i - 15] >> 3);
            const uint32_t s1 = right_rot32(w[i - 2], 17) ^ right_rot32(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        a = hArr[0];
        b = hArr[1]; 
        c = hArr[2];
        d = hArr[3];
        e = hArr[4];
        f = hArr[5];
        g = hArr[6];
        h = hArr[7];

        //Compression Loop:
        for(int i = 0; i < 64; i++){
            const uint32_t s1 = right_rot32(e, 6) ^ right_rot32(e, 11) ^ right_rot32(e, 25);
            const uint32_t ch = (e & f) ^ (~e & g);
            const uint32_t temp1 = h + s1 + ch + k32[i] + w[i];
            const uint32_t s0 = right_rot32(a, 2) ^ right_rot32(a, 13) ^ right_rot32(a, 22);
            const uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            const uint32_t temp2 = s0 + maj;
            h = g;
            g = f;
            f = e; 
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        hArr[0] = hArr[0] + a;
        hArr[1] = hArr[1] + b;
        hArr[2] = hArr[2] + c;
        hArr[3] = hArr[3] + d;
        hArr[4] = hArr[4] + e;
        hArr[5] = hArr[5] + f;
        hArr[6] = hArr[6] + g;
        hArr[7] = hArr[7] + h;
    }    


    //Copy the hash into the passed array
    int shift = 24;
    for(int i = 0; i < 32;i++){
        hash[i] = (uint8_t) (hArr[i / 4] >> shift);
        if(shift == 0)
            shift = 24;
        else
            shift -= 8;
        
    }
    
}
void fsha256(uint8_t hash[32], FILE *input, size_t len) {
    uint32_t hArr[8];
    hArr[0] = 0x6a09e667;
    hArr[1] = 0xbb67ae85;
    hArr[2] = 0x3c6ef372;
    hArr[3] = 0xa54ff53a;
    hArr[4] = 0x510e527f;
    hArr[5] = 0x9b05688c;
    hArr[6] = 0x1f83d9ab;
    hArr[7] = 0x5be0cd19;

    fsha224_256(hash, input, len, hArr);
}

void fsha224(uint8_t hash[28], FILE *input, size_t len){


    uint32_t hArr[8];
    hArr[0] = 0xc1059ed8;
    hArr[1] = 0x367cd507;
    hArr[2] = 0x3070dd17;
    hArr[3] = 0xf70e5939;
    hArr[4] = 0xffc00b31;
    hArr[5] = 0x68581511;
    hArr[6] = 0x64f98fa7;
    hArr[7] = 0xbefa4fa4;

    uint8_t sha256hash[32];
    fsha224_256(sha256hash, input, len, hArr);
    memcpy(hash, sha256hash, sizeof(uint8_t) * 28);
}