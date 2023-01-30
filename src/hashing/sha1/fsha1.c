#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <util.h>
#include <stdio.h>

typedef struct {
	FILE *input;
	uint64_t maxLen;
	uint64_t currentLen;
	bool single_one;
	bool done;
	bool allBytedsRead;
} Sha1Context;


//Generates a 512 bit large chunk for the sha algorithm
static bool generate_chunk(uint8_t chunk[64], Sha1Context *context){
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

void sha1(uint8_t hash[20], FILE *input, size_t len){
    Sha1Context context;
    context.currentLen = 0;
    context.done = false;
    context.input = input;
    context.maxLen = len;
    context.single_one = false;
    context.allBytedsRead = false;

    uint32_t hArr[5]; //h0..h4 
    hArr[0] = 0x67452301;
    hArr[1] = 0xEFCDAB89;
    hArr[2] = 0x98BADCFE;
    hArr[3] = 0x10325476;
    hArr[4] = 0xC3D2E1F0;

    uint8_t chunk[64];
    while(generate_chunk(chunk, &context)){
        uint32_t w[80];

        //copying the data of chunk in the first 16 elements and fill the rest with 0
        const uint8_t *p = chunk;
        memset(w, 0x00, sizeof w);
        for (int i = 0; i < 16; i++) {
            w[i] = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 |
                (uint32_t) p[2] << 8 | (uint32_t) p[3];
            p += 4;
        }
        
        //Extend the 16 32-bit words into 80
        for(int i = 16; i < 80; i++){
            w[i] = left_rot32((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);
        }

        uint32_t a = hArr[0];
        uint32_t b = hArr[1];
        uint32_t c = hArr[2];
        uint32_t d = hArr[3];
        uint32_t e = hArr[4];

        for(int i = 0; i < 80; i++){
            uint32_t f,k;

            if(i >= 0 && i <= 19){
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            if(i >= 20 && i <= 39){
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            if(i >= 40 && i <= 59){
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            if(i >= 60 && i <= 79){
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = left_rot32(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = left_rot32(b, 30);
            b = a;
            a = temp;
        }

        hArr[0] += a;
        hArr[1] += b;
        hArr[2] += c;
        hArr[3] += d;
        hArr[4] += e;
    }

    //Copy the hash into the passed array
    int shift = 24;
    for(int i = 0; i < 20; i++){
        hash[i] = (uint8_t) (hArr[i / 4] >> shift);
        if(shift == 0)
            shift = 24;
        else
            shift -= 8;
    }
}   