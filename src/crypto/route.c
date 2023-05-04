#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <crypto.h>

static void spiral_counterclockwise_encrypt(const char *input, char *dest, int64_t rows){
    size_t inputLen = strlen(input);
    int64_t columns = (inputLen  / rows) + ((inputLen % rows) != 0);

    int64_t destIndex = 0;
    int64_t rotations = 0;
    while (true)
    {   
        //Down
        for(int64_t i = rotations; i < rows - rotations; i++){
            if(!(inputLen <= (i * columns + rotations)))
                dest[destIndex] = input[i * columns + rotations];
            else
                dest[destIndex] = 'X';
            destIndex++;
        }

        if(destIndex == rows * columns)
            break;

        //Right
        for(int64_t i = rotations + 1; i < columns - rotations; i++){
            if(!(inputLen <= (rows - rotations - 1) * columns + i))
                dest[destIndex] = input[((rows - rotations - 1) * columns + i)];
            else
                dest[destIndex] = 'X';

            destIndex++;
        }

        if(destIndex == rows * columns)
            break;

        //Up
        for(int64_t i = rows - rotations - 2; i > rotations - 1; i--){
            if(!(inputLen <= (rows - rotations - 1) * columns + i))
                dest[destIndex] = input[(i * columns + (columns - rotations - 1))];
            else
                dest[destIndex] = 'X';
            destIndex++;
        }
        

        if(destIndex == rows * columns)
            break;
        
        //Left
        for(int64_t i = columns - rotations - 2; i > rotations; i--){
            dest[destIndex] = input[rotations * columns + i];
            destIndex++;
        }

        if(destIndex == rows * columns)
            break;

        rotations++;
    }

    dest[destIndex] = 0;
}

static void spriral_counterclockwise_decrypt(const char *input, char *dest, int64_t rows){
    size_t inputLen = strlen(input);
    int64_t columns = inputLen  / rows;

    int64_t inputIndex = 0;
    int64_t rotations = 0;
    while (true)
    {   
        //Down
        for(int64_t i = rotations; i < rows - rotations; i++){
            dest[i * columns + rotations] = input[inputIndex];
            inputIndex++;
        }

        if(inputIndex == rows * columns)
            break;

        //Right
        for(int64_t i = rotations + 1; i < columns - rotations; i++){
            dest[((rows - rotations - 1) * columns + i)] = input[inputIndex];
            inputIndex++;
        }

        if(inputIndex == rows * columns)
            break;

        //Up
        for(int64_t i = rows - rotations - 2; i > rotations - 1; i--){
            dest[(i * columns + (columns - rotations - 1))] = input[inputIndex];
            inputIndex++;
        }
        

        if(inputIndex == rows * columns)
            break;
        
        //Left
        for(int64_t i = columns - rotations - 2; i > rotations; i--){
            dest[rotations * columns + i] = input[inputIndex];
            inputIndex++;
        }

        if(inputIndex == rows * columns)
            break;

        rotations++;
    }

    dest[inputIndex] = 0;
}

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
    else if(route == SpiralCounterclockwise)
        spiral_counterclockwise_encrypt(input, dest, (int64_t) rows);
}

void route_cipher_decrypt(const char *input, char *dest, Route route, uint32_t rows){
    if(rows == 0)
        return;

    if(route == ZigZagDownUp)
        zigzag_downup_decrypt(input, dest, rows);
    else if(route == ZigZagUpDown)
        zigzag_updown_decrypt(input, dest, rows);
    else if(route == SpiralCounterclockwise)
        spriral_counterclockwise_decrypt(input, dest, (int64_t) rows);
}
