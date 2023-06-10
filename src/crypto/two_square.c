#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include <crypto.h>

//Alphabet without J because J's get geplaced wtih I's in the encryption process
static const char Alphabet[26] = "ABCDEFGHIKLMNOPQRSTUVWXYZ";

//Get pointer to next alphabetic character
static const char *next_alpha(const char *input){
    while (!isalpha(*input) && *input != 0)
        input++;
    return input;
}

static void fill_grid(char grid[25], const char *keyword){
    size_t keywordLen = strlen(keyword);
    int letterCopied = 0;

    for(int i = 0; i < keywordLen; i++){
        int j = 0;
        //Because we replace J with I we have to treat it different
        if(keyword[i] == 'J') {
            for(; j < letterCopied; j++){
                if(grid[j] == 'I')
                    break;
                
            }
        
            if(j == letterCopied){
                grid[letterCopied] = 'I';
                letterCopied++;
                continue;
            }

            continue;
        }


        //Check if letter is already in grid
        for(; j < letterCopied; j++){
            if(grid[j] == keyword[i])
                break;   
        }

        if(j == letterCopied){
            grid[letterCopied] = keyword[i];
            letterCopied++;
        }
    }

    //Fill remaining space with all character that are not already in the grid
    const char *alphabetPtr = Alphabet;
    while(letterCopied != 25){
        int i = 0;
        for(; i < letterCopied; i++)
            if(grid[i] == *alphabetPtr)
                break;

        if(i == letterCopied){
            grid[letterCopied] = *alphabetPtr;
            letterCopied++;
        }

        alphabetPtr++;
    }
}

void two_square_encrypt_decrypt(const char *input, char *dest, const char *keyword1, const char *keyword2, GridPosition pos){
    char grid1[25];
    char grid2[25];
    size_t keyword1Len = strlen(keyword1);

    fill_grid(grid1, keyword1);
    fill_grid(grid2, keyword2);

    //Lookup Tables to get index in Grid of a character
    char lookup1[26];
    char lookup2[26];

    for(int i = 0; i < 25; i++){
        lookup1[grid1[i] - 65] = i; 
        lookup2[grid2[i] - 65] = i; 
    }

    char block[2];
    bool done = false;

    while(input[0] != 0){
        const char *nextAlpha = next_alpha(input);
        if(*nextAlpha == 0)
            break;

        block[0] = toupper(*nextAlpha);
        if(block[0] == 'J')
            block[0] = 'I';
        
        nextAlpha = next_alpha(nextAlpha + 1);
        block[1] = toupper(*nextAlpha);
        if(block[1] == 0){
            block[1] = 'X'; //Pad with X if input size is uneven
            done = true;
        }
        input = nextAlpha+1;

        int char1PosX = lookup1[block[0] - 65] % 5;
        int char2PosX = lookup2[block[1] - 65] % 5;
        int char1PosY = lookup1[block[0] - 65] / 5;
        int char2PosY = lookup2[block[1] - 65] / 5;

        if(!(char1PosY == char2PosY) && pos == Horizontal){
            block[0] = grid1[char2PosY * 5 + char1PosX];
            block[1] = grid2[char1PosY * 5 + char2PosX];
        }

        if(!(char1PosX == char2PosX) && pos == Vertical){
            block[0] = grid1[char1PosY * 5 + char2PosX];
            block[1] = grid2[char2PosY * 5 + char1PosX];
        }

        memcpy(dest, block, 2);

        dest += 2;
        
        if(done)
            break;
    }

    dest[0] = 0;
}