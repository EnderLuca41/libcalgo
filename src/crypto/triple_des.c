#include <stdint.h>
#include <stdlib.h>

void des_encrypt(uint8_t *, uint8_t *, size_t, uint64_t);
void des_decrypt(uint8_t *, uint8_t *, size_t, uint64_t);

void triple_des_encrypt(uint8_t *data, uint8_t *dest, size_t dataLen, uint64_t keys[3]){
    des_encrypt(data, dest, dataLen, keys[0]);
    des_decrypt(dest, dest, (8 - (dataLen % 8)) + dataLen, keys[1]);
    des_encrypt(dest, dest, (8 - (dataLen % 8)) + dataLen, keys[2]);
}

void triple_des_decrypt(uint8_t *data, uint8_t *dest, size_t dataLen, uint64_t keys[3]){
    des_decrypt(data, dest, dataLen, keys[2]);
    des_encrypt(dest, dest, dataLen, keys[1]);
    des_decrypt(dest, dest, dataLen, keys[0]);
}