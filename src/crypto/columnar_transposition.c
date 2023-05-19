#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Function assumes that keyword stores a 7bit big ascii character
 * Returns false if a character is repeated in keyword
 */
bool columnar_transpostion_encrypt(const char *input, char *dest, const char *keyword){
    uint8_t keywordLen = 0;

    uint8_t i = 0;
    for(; keyword[i] != 0; i++){
        keywordLen++;
        uint8_t j = 0;
        for(; j < i; j++){
            if(keyword[i] == keyword[j])
                break;
        }

        if(j != i)
            break;
    }

    //Return if a character is more than one time in keyword
    if(keyword[i] != 0)
        return false;

    //Setup lookup table to get index of a character in keyword
    char lookup[128];
    memset(lookup, -1, 128);
    for(i = 0; i < keywordLen; i++){
        lookup[keyword[i]] = i;
    }

    uint8_t *alphabetOrder = malloc(keywordLen);
    
    //Fill alphabetOrder
    uint8_t alphabetOrderPtr = 0;
    for(i = 1; i < 128; i++){
        if(lookup[i] != -1){
            alphabetOrder[lookup[i]] = alphabetOrderPtr;
            alphabetOrderPtr++;
        }
    }

    size_t inputLen = strlen(input);
    uint32_t rows = inputLen / keywordLen + (inputLen % keywordLen != 0);
    uint8_t columns = keywordLen;

    //Main encryption Loop
    for(i = 0; i < columns; i++){
        for(uint32_t j = 0; j < rows; j++){
            if(j * columns + i >= inputLen) {
                //Pad with X's if the grid is bigger than input
                dest[alphabetOrder[i] * rows + j] = 'X';
                continue;
            }

            dest[alphabetOrder[i] * rows + j] = input[j * columns + i];
        }
    }

    dest[rows * keywordLen] = 0;

    free(alphabetOrder);
    return true;
}

void columnar_transposition_decrypt(const char *input, char *dest, const char *keyword){ 
    //No check if a character is repeated becuase the keyword has already
    //been checked in the encryption process
    uint8_t keywordLen = strlen(keyword);

    //Setup lookup table to get index of a character in keyword
    char lookup[128];
    memset(lookup, -1, 128);
    for(uint8_t i = 0; i < keywordLen; i++){
        lookup[keyword[i]] = i;
    }

    uint8_t *alphabetOrder = malloc(keywordLen);
    
    //Fill alphabetOrder
    uint8_t alphabetOrderPtr = 0;
    for(uint8_t i = 1; i < 128; i++){
        if(lookup[i] != -1){
            alphabetOrder[lookup[i]] = alphabetOrderPtr;
            alphabetOrderPtr++;
        }
    }


    size_t inputLen = strlen(input);
    uint8_t columns = keywordLen;
    uint32_t rows = inputLen / columns;

    //Main decryption Loop
    for(uint8_t i = 0; i < columns; i++){
        for(uint32_t j = 0; j < rows; j++){
            dest[j * columns + i] = input[alphabetOrder[i] * rows + j];
        }
    }
    dest[inputLen] = 0;

    free(alphabetOrder);
}