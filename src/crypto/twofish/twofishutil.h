#ifndef TWOFISHUTIL_H
#define TWOFISHUTIL_H

#include <stdint.h>

typedef struct 
{
    uint32_t s0[256];
    uint32_t s1[256];
    uint32_t s2[256];
    uint32_t s3[256];
    uint32_t subkeys[40];
} TwofishContext;

#ifdef __cplusplus
extern "C" {
#endif

void twofish_init(const uint8_t *key, uint8_t keyLen, TwofishContext *ctx);

#ifdef __cplusplus
}
#endif

#endif
