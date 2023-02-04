#include <stdlib.h>
#include <stdint.h>
#include <util.h>

#define mix(a,b,c) \
{ \
  a -= c;  a ^= left_rot32(c, 4);  c += b; \
  b -= a;  b ^= left_rot32(a, 6);  a += c; \
  c -= b;  c ^= left_rot32(b, 8);  b += a; \
  a -= c;  a ^= left_rot32(c, 16);  c += b; \
  b -= a;  b ^= left_rot32(a, 19);  a += c; \
  c -= b;  c ^= left_rot32(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= left_rot32(b, 14); \
  a ^= c; a -= left_rot32(c, 11); \
  b ^= a; b -= left_rot32(a, 25); \
  c ^= b; c -= left_rot32(b, 16); \
  a ^= c; a -= left_rot32(c, 4);  \
  b ^= a; b -= left_rot32(a, 14); \
  c ^= b; c -= left_rot32(b, 24); \
}

uint32_t lookup3(const void *input, size_t len){
    uint32_t a,b,c;
    union { const void *ptr; size_t i; } u; 

    a = b = c = 0xdeadbeef + ((uint32_t) len);
    u.ptr = input;
    
    const uint32_t *k = (const uint32_t *)input;
    const uint8_t *k8;

    while(len > 12){
        a += k[0];
        b += k[1];
        c += k[2];
        mix(a, b, c);
        len -= 12;
        k += 3;
    }

    k8 = (const uint8_t *)k;
    switch (len)
    {
        case 12: c += k[2]; b += k[1]; a += k[0]; break;
        case 11: c += ((uint32_t) k8[10]) << 16;
        case 10: c += ((uint32_t) k8[9]) << 8;
        case 9: c += k8[8];
        case 8: b += k[1]; a += k[0]; break;
        case 7: b += ((uint32_t) k8[6]) << 16;
        case 6: b += ((uint32_t) k8[5]) << 8;
        case 5: b += k8[4];
        case 4: a += k[0]; break;
        case 3: a += ((uint32_t) k8[2]) << 16;
        case 2: a += ((uint32_t) k8[1]) << 8;
        case 1: a += k8[0]; break;
        case 0: return c;
    }

    final(a, b, c);
    return c;
}