#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Function assumes that keyword stores a 7bit big ascii character
 * Returns false if a character is repeated in keyword or a allocation fails
 */
bool columnar_transpostion_encrypt(const char *input, char *dest, const char *keyword){
    if(*input == '\0') 
        return true; //Empty string, no error or false input

    uint8_t keywordLen = 0;

    //Loop trough the code, if a character is reapead break
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

    //check if i reached the null terminator,
    //if not a character is repeated
    if(keyword[i] != 0)
        return false;

    //Setup lookup table to get index of a character in keyword
    char lookup[256];
    memset(lookup, -1, 256);
    for(i = 0; i < keywordLen; i++){
        lookup[keyword[i]] = i;
    }

    uint8_t *alphabetOrder = malloc(keywordLen);
    if(alphabetOrder == NULL)
        return false;
    
    //Fill with the alphabetic order position of each character in keyowrd
    uint8_t alphabetOrderPtr = 0;
    for(i = 1; i != 0; i++){
        if(lookup[i] != -1){
            alphabetOrder[lookup[i]] = alphabetOrderPtr;
            alphabetOrderPtr++;
        }
    }

    size_t inputLen = strlen(input);
    uint32_t rows = inputLen / keywordLen + (inputLen % keywordLen != 0);
    uint8_t columns = keywordLen;

    for(i = 0; i < columns; i++){
        //Calculate how many columns have been shorter before,
        //to subtract them later from the index
        uint32_t shorterColumns = 0;
        if(alphabetOrder[i] > inputLen % columns)
            shorterColumns = alphabetOrder[i] - columns - shorterColumns + 1;

        for(uint32_t j = 0; j < rows; j++){
            if(j * columns + i >= inputLen)
                continue;
            
            dest[alphabetOrder[i] * rows + j - shorterColumns] = input[j * columns + i];
        }

    }

    dest[inputLen] = '\0';

    free(alphabetOrder);
    return true;
}

void columnar_transposition_decrypt(const char *input, char *dest, const char *keyword){ 
    //No check if a character is repeated becuase the keyword has already
    //been checked in the encryption process
    uint8_t keywordLen = strlen(keyword);

    //Setup lookup table to get index of a character in keyword
    char lookup[256];
    memset(lookup, -1, 256);
    for(uint8_t i = 0; i < keywordLen; i++){
        lookup[keyword[i]] = i;
    }

    uint8_t *alphabetOrder = malloc(keywordLen);
    
    //Fill with the alphabetic order position of each character in keyowrd
    uint8_t alphabetOrderPtr = 0;
    for(uint8_t i = 1; i != 0; i++){
        if(lookup[i] != -1){
            alphabetOrder[lookup[i]] = alphabetOrderPtr;
            alphabetOrderPtr++;
        }
    }


    size_t inputLen = strlen(input);
    uint32_t rows = inputLen / keywordLen + (inputLen % keywordLen != 0);
    uint8_t columns = keywordLen;

    
    //Main decryption Loop
    for(uint8_t i = 0; i < columns; i++){
        //Calculate how many columns have been shorter before,
        //to subtract them later from the index
        uint8_t shorterColumns;
        if(alphabetOrder[i] > inputLen % columns)
            shorterColumns = alphabetOrder[i] - inputLen % columns + 1;
        else
            shorterColumns = 0;

        for(uint32_t j = 0; j < rows; j++){
            if(j * columns + i >= inputLen)
                continue;
            dest[j * columns + i] = input[alphabetOrder[i] * rows + j - shorterColumns];
        }
    }
    dest[inputLen] = '\0';

    free(alphabetOrder);
}
