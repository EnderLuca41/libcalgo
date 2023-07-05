#ifndef BLOWFISHUTIL_H
#define BLOWFISHUTIL_H

#include <stdint.h>

typedef struct 
{
    uint32_t subkeys[18];
    uint32_t s1[256];
    uint32_t s2[256];
    uint32_t s3[256];
    uint32_t s4[256];
} BlowfishContext;

#ifdef __cplusplus
extern "C" {
#endif

void encrypt_block(uint32_t *l, uint32_t *r, const BlowfishContext *ctx);
void decrypt_block(uint32_t *l, uint32_t *r, const BlowfishContext *ctx);

//Generate Subkeys and Key-dependent S-Boxes
void blowfish_init(const uint8_t *key, uint8_t keyLen, BlowfishContext *ctx);

//Generate 8 byte big block, pad with zeros if input has less than 8 bytes
size_t generate_block_blowfish(uint32_t *l, uint32_t *r, const uint8_t *input, size_t inputSize);

#ifdef __cplusplus
}
#endif

#endif
