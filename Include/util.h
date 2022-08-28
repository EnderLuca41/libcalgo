#ifndef UTIL_H
#define UTIL_H

#define Swap8Bytes(val) \
((((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
(((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | \
(((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
(((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )

#ifdef __cplusplus
extern "C"{
#endif
    inline uint32_t right_rot32(uint32_t x, uint8_t n) { return (x >> n) | (x << (32-n)); }
    inline uint64_t right_rot64(uint64_t x, uint8_t n) { return (x >> n) | (x << (64-n)); }
#ifdef __cplusplus
}
#endif

#endif