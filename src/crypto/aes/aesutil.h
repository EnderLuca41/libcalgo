#ifndef AESUTIL_H
#define AESUTIL_H

#include <stdint.h>

const extern uint8_t S_RijndaelInverse[256];
const extern uint8_t S_Rijndael[256];

const extern uint8_t Rc[256];

const extern uint8_t Mult2[256];
const extern uint8_t Mult3[256];
const extern uint8_t Mult9[256];
const extern uint8_t Mult11[256];
const extern uint8_t Mult13[256];
const extern uint8_t Mult14[256];


#ifdef __cplusplus
extern "C" {
#endif

void rotword(uint8_t [4]);
void subword(uint8_t [4]);

void mix_column(uint8_t [4]);
void undo_mix_column(uint8_t [4]);


#ifdef __cplusplus
}
#endif

#endif