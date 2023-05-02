#ifndef CRYPTO_H
#define CRYPTO_H

typedef enum{
    /*
     * 01 08 09
     * 02 07 10
     * 03 06 11
     * 04 05 12
     */
    ZigZagDownUp,

    /*
     * 04 05 12
     * 03 06 11
     * 02 07 10
     * 01 08 09
     */
    ZigZagUpDown
} Route;

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif