#include <stdint.h>
#include <util.h>

#define mix(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c >> 13); \
  b -= c; b -= a; b ^= (a << 8); \
  c -= a; c -= b; c ^= (b >> 13); \
  a -= b; a -= c; a ^= (c >> 12);  \
  b -= c; b -= a; b ^= (a << 16); \
  c -= a; c -= b; c ^= (b >> 5); \
  a -= b; a -= c; a ^= (c >> 3);  \
  b -= c; b -= a; b ^= (a << 10); \
  c -= a; c -= b; c ^= (b >> 15); \
}

uint32_t lookup2(const uint8_t *input, uint32_t len){
    uint32_t l = len;
    uint32_t a,b,c;
    a = b = 0x9e3779b9;
    c = 0;

    while(l >= 12){
        a += (input[0] + ((uint32_t) input[1] << 8) + 
            ((uint32_t) input[2] << 16) + ((uint32_t) input[3] << 24));
        b += (input[4] + ((uint32_t) input[5] << 8) + 
            ((uint32_t) input[6] << 16) + ((uint32_t) input[7] << 24));
        c += (input[8] + ((uint32_t) input[9] << 8) + 
            ((uint32_t) input[10] << 16) + ((uint32_t) input[11] << 24));

        mix(a, b, c);
        input += 12;
        l -= 12;
    }

    c += len;

    switch(l){
        case 11: c += ((uint32_t) input[10] << 24);
        case 10: c += ((uint32_t)input[9] << 16);
        case 9: c += ((uint32_t) input[8] << 8);
        case 8: b += ((uint32_t) input[7] << 24);
        case 7: b += ((uint32_t) input[6] << 16);
        case 6: b += ((uint32_t) input[5] << 8);
        case 5: b += input[4];
        case 4: a += ((uint32_t) input[3] << 24);
        case 3: a += ((uint32_t) input[2] << 16);
        case 2: a += ((uint32_t) input[1] << 8);
        case 1: a += input[0];
    }

    mix(a, b, c);
    return c;
}