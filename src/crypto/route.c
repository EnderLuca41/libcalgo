#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <crypto.h>

static void zigzag_downup_encrypt(const char *input, char *dest, uint32_t rows){
    size_t inputLen = strlen(input);
    uint32_t columns = (inputLen  / rows) + ((inputLen % rows) != 0);

    uint32_t add = 1;
    uint32_t j = 0;
    for(uint32_t i = 0; i < columns; i++){
        
        while (true)
        {
            //Check if position is out of bounds
            if(!(inputLen <= (j * columns + i))) 
                *dest = input[j * columns + i];
            else
                *dest = 'X';

            dest += 1;
            
            if((j+add == UINT32_MAX) || (j+add == rows))
                break;

            j += add;
        }

        add = -add;
    }
    *dest = 0;
}

static void zigzag_downup_decrypt(const char *input, char *dest, uint32_t rows){
    size_t inputLen = strlen(input);
    uint32_t columns = inputLen / rows;

    uint32_t add = 1;
    uint32_t j = 0;
    for(uint32_t i = 0; i < columns; i++){
        while (true)
        {
            dest[j * columns + i] = *input;

            input += 1;
            
            if((j+add == UINT32_MAX) || (j+add == rows))
                break;

            j += add;
        }

        add = -add;
    }

    dest[inputLen] = 0;
}

static void zigzag_updown_encrypt(const char *input, char *dest, uint32_t rows){
    size_t inputLen = strlen(input);
    uint32_t columns = (inputLen  / rows) + ((inputLen % rows) != 0);

    uint32_t add = -1;
    uint32_t j = rows - 1;
    for(uint32_t i = 0; i < columns; i++){
        
        while (true)
        {
            //Check if position is out of bounds
            if(!(inputLen <= (j * columns + i))) 
                *dest = input[j * columns + i];
            else
                *dest = 'X';

            dest += 1;
            
            if((j+add == UINT32_MAX) || (j+add == rows))
                break;

            j += add;
        }

        add = -add;
    }
    *dest = 0;
}

static void zigzag_updown_decrypt(const char *input, char *dest, uint32_t rows){
    size_t inputLen = strlen(input);
    uint32_t columns = inputLen / rows;

    uint32_t add = -1;
    uint32_t j = rows - 1;
    for(uint32_t i = 0; i < columns; i++){
        while (true)
        {
            dest[j * columns + i] = *input;

            input += 1;
            
            if((j+add == UINT32_MAX) || (j+add == rows))
                break;

            j += add;
        }

        add = -add;
    }

    dest[inputLen] = 0;
}


void route_cipher_encrypt(const char *input, char *dest, Route route, uint32_t rows){
    if(rows == 0)
        return;

    if(route == ZigZagDownUp)
        zigzag_downup_encrypt(input, dest, rows);
    else if(route == ZigZagUpDown)
        zigzag_updown_encrypt(input, dest, rows);
}

void route_cipher_decrypt(const char *input, char *dest, Route route, uint32_t rows){
    if(rows == 0)
        return;

    if(route == ZigZagDownUp)
        zigzag_downup_decrypt(input, dest, rows);
    else if(route == ZigZagUpDown)
        zigzag_updown_decrypt(input, dest, rows);
}
