#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool columnar_transpostion_encrypt(const char *, char *, const char *);
bool columnar_transposition_decrypt(const char *, char *, const char *);

const static char Adfgx[5] = {'A', 'D', 'F', 'G', 'X'};
const static char StdSquare[26] = "ABCDEFGHIKLMNOPQRSTUVWXYZ";

/*
 * Returns false if key has a non alphabetic character or a character appears more than on time
 * Square has to contain all alphabetic characters except for J
 * If NULL is passed for square, a standard alphabet will be used
 */
bool adfgx_encrypt(const char *text, char *dest, const char square[26], const char *key){
    size_t keyLen = strlen(key);
    if(keyLen > 26)
        return false;

    //Check if a character appears twice and if key holds any non alphabetic characters
    for(uint8_t i = 0; i < keyLen; i++){
        if(!isalpha(key[i]))
            return false;

        for(uint8_t j = 0; j < i; j++){
            if(toupper(key[i]) == toupper(key[j]))
                return false;
        }
    }

    //Use standard alphabet
    if(square == NULL)
        square = StdSquare;

    //define a lookup table for the polybius square to get the position by the character
    char lookup[26];
    for(char i = 0; i < 25; i++){
        lookup[toupper(square[i]) - 'A'] = i;
    }

    char *fractionated = malloc(100);
    if(fractionated == NULL)
        return false;
    
    size_t fractionatedSize = 100;
    size_t fractionatedIndex = 0;

    for(;*text != '\0'; text++){
        if(!isalpha(*text))
            continue;

        char current = toupper(*text);
        if(current == 'J')
            current = 'I';

        uint8_t posX = lookup[current - 'A'] % 5;
        uint8_t posY = lookup[current - 'A'] / 5;
        fractionated[fractionatedIndex] = Adfgx[posY];
        fractionated[fractionatedIndex+1] = Adfgx[posX];

        fractionatedIndex += 2;

        if((fractionatedIndex+1) % 100 == 0){
            fractionatedSize += 100;
            char *buff = realloc(fractionated, fractionatedSize);
            if(buff == NULL){
                free(fractionated);
                return false;
            }
            fractionated = buff;
        }
    }

    fractionated[fractionatedIndex] = '\0';
    
    columnar_transpostion_encrypt(fractionated, dest, key);

    free(fractionated);
    return true;
}

const static char lookup[] = {0, -1, -1, 1, -1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1};

void adfgx_decrypt(const char *text, char *dest, const char square[26], const char *key){
    if(square == NULL)
        square = StdSquare;

    size_t inputLen = strlen(text);
    char *buff = malloc(inputLen + 1);
    columnar_transposition_decrypt(text, buff, key);

    size_t buffCounter = 0;
    while(buff[buffCounter] != '\0'){
        uint8_t posY = lookup[buff[buffCounter] - 'A'];
        uint8_t posX = lookup[buff[buffCounter+1] - 'A'];
        
        *dest = square[posY * 5 + posX];
        buffCounter += 2;
        dest++;
    }
    *dest = '\0';

    free(buff);
}