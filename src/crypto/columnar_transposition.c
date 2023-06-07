#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
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

    size_t inputLen = strlen(input);
    uint32_t rows = inputLen / keywordLen + (inputLen % keywordLen != 0);
    uint8_t columns = keywordLen;

    uint8_t *alphabetOrder = malloc(keywordLen);
    uint8_t *shorterColumns = malloc(keywordLen);
    if(alphabetOrder == NULL || shorterColumns == NULL)
        return false;
    
    //Fill with the alphabetic order position of each character in keyowrd
    //And determine the amount of shorter columns before each character
    uint8_t shorterColumnsTrack = 0;
    uint8_t alphabetOrderPtr = 0;
    for(uint8_t i = 1; i != 0; i++){
        if(lookup[i] != -1){
            alphabetOrder[lookup[i]] = alphabetOrderPtr;
            
            //Set how many columns have been shorter before
            shorterColumns[alphabetOrderPtr] = shorterColumnsTrack;

            //Increase if the current colums is shorter
            if(lookup[i] >= inputLen % columns){
                shorterColumnsTrack++;
            }

            alphabetOrderPtr++;
        }
    }

    //Main Encryption Loop
    for(i = 0; i < columns; i++){
        for(uint32_t j = 0; j < rows; j++){
            if(j * columns + i >= inputLen)
                continue;

            dest[alphabetOrder[i] * rows + j - shorterColumns[alphabetOrder[i]]] = input[j * columns + i];
        }
    }

    dest[inputLen] = '\0';

    free(alphabetOrder);
    free(shorterColumns);
    return true;
}

/*
 * Returns false if a allocation fails
 */
bool columnar_transposition_decrypt(const char *input, char *dest, const char *keyword){ 
    //No check if a character is repeated becuase the keyword has already
    //been checked in the encryption process
    uint8_t keywordLen = strlen(keyword);

    //Setup lookup table to get index of a character in keyword
    char lookup[256];
    memset(lookup, -1, 256);
    for(uint8_t i = 0; i < keywordLen; i++){
        lookup[keyword[i]] = i;
    }

    size_t inputLen = strlen(input);
    uint32_t rows = inputLen / keywordLen + (inputLen % keywordLen != 0);
    uint8_t columns = keywordLen;

    uint8_t *alphabetOrder = malloc(keywordLen);
    uint8_t *shorterColumns = malloc(keywordLen);
    if(alphabetOrder == NULL || shorterColumns == NULL)
        return false;
    
    //Fill with the alphabetic order position of each character in keyowrd
    //And determine the amount of shorter columns before each character
    uint8_t shorterColumnsTrack = 0;
    uint8_t alphabetOrderPtr = 0;
    for(uint8_t i = 1; i != 0; i++){
        if(lookup[i] != -1){
            alphabetOrder[lookup[i]] = alphabetOrderPtr;
            
            //Set how many columns have been shorter before
            shorterColumns[alphabetOrderPtr] = shorterColumnsTrack;

            //Increase if the current colums is shorter
            if(lookup[i] >= inputLen % columns){
                shorterColumnsTrack++;
            }

            alphabetOrderPtr++;
        }
    }
    
    //Main decryption Loop
    for(uint8_t i = 0; i < columns; i++){
        for(uint32_t j = 0; j < rows; j++){
            if(j * columns + i >= inputLen)
                continue;
            dest[j * columns + i] = input[alphabetOrder[i] * rows + j - shorterColumns[alphabetOrder[i]]];
        }
    }

    dest[inputLen] = '\0';

    free(alphabetOrder);
    free(shorterColumns);
    return true;
}