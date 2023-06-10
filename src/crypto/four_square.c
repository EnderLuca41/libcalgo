#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

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

void four_square_encrypt(const char *input, char *dest, const char *keyword1, const char *keyword2){
    char grid1[25];
    char grid2[25];

    fill_grid(grid1, keyword1);
    fill_grid(grid2, keyword2);

    bool done = false;

    while(input[0] != 0){
        const char *nextAlpha = next_alpha(input);
        if(*nextAlpha == 0)
            break;

        dest[0] = toupper(*nextAlpha);
        if(dest[0] == 'J')
            dest[0] = 'I';
        
        nextAlpha = next_alpha(nextAlpha + 1);
        dest[1] = toupper(*nextAlpha);
        if(dest[1] == 0){
            dest[1] = 'X'; //Pad with X if input size is uneven
            done = true;
        }
        input = nextAlpha+1;

        int char1Pos = dest[0] - 65;
        int char2Pos = dest[1] - 65;

        if(char1Pos >= 9)
            char1Pos--;
        if(char2Pos >= 9)
            char2Pos--;

        dest[0] = grid1[char2Pos % 5 + char1Pos / 5 * 5];
        dest[1] = grid2[char1Pos % 5 + char2Pos / 5 * 5];

        dest += 2;
        if(done)
            break;
    }

    dest[0] = 0;
}

void four_square_decrypt(const char *input, char *dest, const char *keyword1, const char *keyword2){
    char grid1[25];
    char grid2[25];
    
    fill_grid(grid1, keyword1);
    fill_grid(grid2, keyword2);

    //Create lookup tables to get the position in Grid of a character
    char lookup1[26];
    char lookup2[26]; 
    for(int i = 0; i < 25; i++){
        lookup1[grid1[i] - 65] = i;
        lookup2[grid2[i] - 65] = i;
    }

    while(input[0] != 0){
        memcpy(dest, input, 2);

        int char1Pos = lookup1[dest[0] - 65];
        int char2Pos = lookup2[dest[1] - 65];

        dest[0] = Alphabet[char1Pos / 5 * 5 + char2Pos % 5];
        dest[1] = Alphabet[char2Pos / 5 * 5 + char1Pos % 5];
        input += 2;
        dest += 2;
    }

    dest[0] = 0;
}
