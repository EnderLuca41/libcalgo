#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <memory.h>
#include <util.h>
#include <stdio.h>

static const uint64_t k[80] = {
    0x428a2f98d728ae22u, 0x7137449123ef65cdu,
	0xb5c0fbcfec4d3b2fu, 0xe9b5dba58189dbbcu,
	0x3956c25bf348b538u, 0x59f111f1b605d019u,
	0x923f82a4af194f9bu, 0xab1c5ed5da6d8118u,
	0xd807aa98a3030242u, 0x12835b0145706fbeu,
	0x243185be4ee4b28cu, 0x550c7dc3d5ffb4e2u,
	0x72be5d74f27b896fu, 0x80deb1fe3b1696b1u,
	0x9bdc06a725c71235u, 0xc19bf174cf692694u,
	0xe49b69c19ef14ad2u, 0xefbe4786384f25e3u,
	0x0fc19dc68b8cd5b5u, 0x240ca1cc77ac9c65u,
	0x2de92c6f592b0275u, 0x4a7484aa6ea6e483u,
	0x5cb0a9dcbd41fbd4u, 0x76f988da831153b5u,
	0x983e5152ee66dfabu, 0xa831c66d2db43210u,
	0xb00327c898fb213fu, 0xbf597fc7beef0ee4u,
	0xc6e00bf33da88fc2u, 0xd5a79147930aa725u,
	0x06ca6351e003826fu, 0x142929670a0e6e70u,
	0x27b70a8546d22ffcu, 0x2e1b21385c26c926u,
	0x4d2c6dfc5ac42aedu, 0x53380d139d95b3dfu,
	0x650a73548baf63deu, 0x766a0abb3c77b2a8u,
	0x81c2c92e47edaee6u, 0x92722c851482353bu,
	0xa2bfe8a14cf10364u, 0xa81a664bbc423001u,
	0xc24b8b70d0f89791u, 0xc76c51a30654be30u,
	0xd192e819d6ef5218u, 0xd69906245565a910u,
	0xf40e35855771202au, 0x106aa07032bbd1b8u,
	0x19a4c116b8d2d0c8u, 0x1e376c085141ab53u,
	0x2748774cdf8eeb99u, 0x34b0bcb5e19b48a8u,
	0x391c0cb3c5c95a63u, 0x4ed8aa4ae3418acbu,
	0x5b9cca4f7763e373u, 0x682e6ff3d6b2b8a3u,
	0x748f82ee5defb2fcu, 0x78a5636f43172f60u,
	0x84c87814a1f0ab72u, 0x8cc702081a6439ecu,
	0x90befffa23631e28u, 0xa4506cebde82bde9u,
	0xbef9a3f7b2c67915u, 0xc67178f2e372532bu,
	0xca273eceea26619cu, 0xd186b8c721c0c207u,
	0xeada7dd6cde0eb1eu, 0xf57d4f7fee6ed178u,
	0x06f067aa72176fbau, 0x0a637dc5a2c898a6u,
	0x113f9804bef90daeu, 0x1b710b35131c471bu,
	0x28db77f523047d84u, 0x32caab7b40c72493u,
	0x3c9ebe0a15c9bebcu, 0x431d67c49c100d4cu,
	0x4cc5d4becb3e42b6u, 0x597f299cfc657e2au,
	0x5fcb6fab3ad6faecu, 0x6c44198c4a475817u 
};

typedef struct
{
	const void *input;
	uint64_t inputLen;
	uint64_t currentLen;
	bool done;
	bool single_one;
} Sha512Context;

//Generates a 1024 bit large chunk for the sha algorithm
static bool generate_chunk(uint8_t chunk[128], Sha512Context *context){
	if(context->done)
		return false;

	//When there is still 128 bytes or more of the input left,
    //128 bytes get simply copied into chunk
	if(context->currentLen >= 128){
		memcpy(chunk, context->input, 128);
		context->input += 128;
		context->currentLen -= 128;
		return true;
	}

    //When there is 127 bytes of data or less left they get all copied into chunk
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

	//If there is not enough room in the chunk (16 bytes) for the length of the input,
    //The algoritm will pad the remaining space with zero-bytes
    if(space_in_chunk > 112)
    {
        memset(chunk, 0, 128 - space_in_chunk);
        return true;
    }

	/*
    * if the destionation has enough space in the chunk, all still free space except the last 8 bytes get assigned to zero-byte
    * The last 16 bytes are reserved for the size of the input but because the input can't be larger then 2^64 byters we just assign the before last 8 bytes also zero
    * The last 8 bytes then get filled with the length of the input in Big Endian
    */
    memset(chunk, 0, 128 - space_in_chunk - 8);
    chunk += 128 - space_in_chunk - 8;

	//Copy length endian indpendent
	unsigned long len = context->inputLen * 8;
	uint8_t shift = 56;
	for(uint8_t i = 0; i < 8; i++){
		chunk[i] = (uint8_t) (len >> shift);
		shift -= 8;
	}
    
    context->done = true;
    return true;
}

static void sha384_512(uint8_t hash[64], const void *input, size_t len, const uint64_t Hs[8]){
	uint64_t hArr[8];
	memcpy(hArr, Hs, sizeof(uint64_t) * 8);

	Sha512Context
 context;
	context.currentLen = len;
	context.inputLen = len;
	context.input = input;
	context.done = false;
	context.single_one = false;

	uint8_t chunk[128]; 
	while(generate_chunk(chunk, &context)){
		uint64_t w[80];
		uint64_t a,b,c,d,e,f,g,h;
		uint8_t i;

	    //copying the data of chunk in the first 32 elements and fill the rest with 0
		const uint8_t *p = chunk;
        memset(w, 0x00, sizeof w);
        for (i = 0; i < 16; i++) {
            w[i] = (uint64_t) p[0] << 56 | (uint64_t) p[1] << 48 | (uint64_t) p[2] << 40 |
			    (uint64_t) p[3] << 32 | (uint64_t) p[4] << 24 | (uint64_t) p[5] << 16 | 
				(uint64_t) p[6] << 8 | (uint64_t) p[7];
            p += 8;
        }

		for(i = 16; i < 80; i++){
			const uint64_t s0 = right_rot64(w[i - 15], 1) ^ right_rot64(w[i - 15], 8) ^ (w[i - 15] >> 7);	
			const uint64_t s1 = right_rot64(w[i - 2], 19) ^ right_rot64(w[i - 2], 61) ^ (w[i - 2] >> 6);
			w[i] = s1 + w[i - 7] + s0 + w[i - 16];
		}
		
		a = hArr[0];
        b = hArr[1];
        c = hArr[2];
        d = hArr[3];
        e = hArr[4];
        f = hArr[5];
        g = hArr[6];
		h = hArr[7];
		//Compression loop:
		for(i = 0; i < 80; i++){
			const uint64_t s1 = right_rot64(e, 14) ^ right_rot64(e, 18) ^ right_rot64(e, 41);
			const uint64_t ch = (e & f) ^ (~e & g);
			const uint64_t temp1 = h + s1 + ch + k[i] + w[i];
			const uint64_t s0 = right_rot64(a, 28) ^ right_rot64(a, 34) ^ right_rot64(a, 39);
			const uint64_t maj = (a & b) ^ (a & c) ^ (b & c);
			const uint64_t temp2 = s0 + maj;
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

	int shift = 56;
	for(uint8_t i = 0; i < 64; i++){
		hash[i] = (uint8_t) (hArr[i / 8] >> shift);
		if(shift == 0)
			shift = 56;
		else
			shift -= 8;
	}
}

void sha512(uint8_t hash[64], const void *input, size_t len){
	const uint64_t h[8] = {
	0x6a09e667f3bcc908u,
	0xbb67ae8584caa73bu,
	0x3c6ef372fe94f82bu,
	0xa54ff53a5f1d36f1u,
	0x510e527fade682d1u,
	0x9b05688c2b3e6c1fu,
	0x1f83d9abfb41bd6bu,
	0x5be0cd19137e2179u
	};

    sha384_512(hash, input, len, h);
}

void sha384(uint8_t hash[48], const void *input, size_t len){
	const uint64_t h[8] = {
		0xCBBB9D5DC1059ED8,
		0x629A292A367CD507,
		0x9159015A3070DD17,
		0x152FECD8F70E5939,
		0x67332667FFC00B31,
		0x8EB44A8768581511,
		0xDB0C2E0D64F98FA7,
		0x47B5481DBEFA4FA4
	};

    uint8_t sha512hash[64];
	sha384_512(sha512hash, input, len, h);
	memcpy(hash, sha512hash, 48);
}