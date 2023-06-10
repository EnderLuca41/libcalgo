#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Alphabet without J because J's get geplaced wtih I's in the encryption process
const char Alphabet[26] = "ABCDEFGHIKLMNOPQRSTUVWXYZ";

//Find the next alphabetic character and return a pointer to it
const char *next_alpha(const char *input){
    while (!isalpha(*input) && *input != 0)
        input++;
    return input;
}

typedef struct 
{
    const char *input;
    bool skipOne;
    int toAddInput;
    int toAddDest;
    bool done;
} PlayfairState;

static bool generate_chunk(PlayfairState *state, char *data1, char *data2){
    state->skipOne = false;
    
    if(state->done)
        return false;
    
    if(state->input[0] == 0)
        return false;
        
    if(!isalpha(state-> input[0])){
        state->skipOne = true;
        return true;
    }

    if(toupper(state->input[0]) == toupper(state->input[1])){
        *data1 = toupper(state->input[0]);
        *data2 = 'X';
        state->toAddInput = 1;
        state->toAddDest = 2;
        return true;
    }

    if(state->input[1] == 0){
        state->done = true;
        *data1 = toupper(state->input[0]);
        *data2 = 'X';
        state->toAddInput = 1;
        state->toAddDest = 2;
        return true;
    }

    if(!isalpha(state->input[1])){
        *data1 = toupper(state->input[0]);
        const char *ptr = next_alpha(state->input + 1);
        if(*ptr == 0){
            state->toAddInput = 2;
            state->toAddDest = 2;
            *data2 = 'X';
            state->done = true;
        }
        else{
            size_t diff = ptr - state->input;
            state->toAddInput = diff + 1;
            state->toAddDest = 2;
            *data2 = toupper(*ptr);
        }
        return true;
    }

    *data1 = toupper(state->input[0]);
    *data2 = toupper(state->input[1]);
    state->toAddInput = 2;
    state->toAddDest = 2;
    return true;
}

/*
 * Encrypts using the Playfair Cipher by Charles Wheatstone
 * Note that information about all other Characters that are not alphabetic get lost
 * This also includes Spaces and Line breaks
 * Because the modified polybius table can only hold 25 Characters, the letter J gets replace by I
 * Return value ist valid C string with a ending zero (\0)
 * If the characters in a 2 byte wide block are the same, then a X is place between them
 * If the input length if odd then a X is appended
 */
void playfair_encrypt(const char *input, char *dest, const char *keyword)
{
    char table[25];
    memset(table, 0, 25);
    size_t keywordLen = strlen(keyword);

    int letterCopied = 0;
    int keywordPtr = 0;
    // Copy keyword into table
    while (letterCopied != 25)
    {  
    
        if (keyword[keywordPtr] == 0)
            break;

        //Check if keyword is J and add I if not so
        if(keyword[keywordPtr] == 'J'){
            int j = 0;
            for (; j < keywordPtr; j++)
            {
                if (table[keywordPtr] == 'I')
                    break;
            }

            if (j == keywordPtr)
            {
                table[letterCopied] = 'I';
                letterCopied++;
                keywordPtr++;
                continue;
            }
        }

        //Check if letter is alreay in table
        int j = 0;
        for (; j < keywordPtr; j++)
        {
            if (table[j] == keyword[keywordPtr])
                break;
        }

        if (j == keywordPtr)
        {
            table[letterCopied] = keyword[keywordPtr];
            letterCopied++;
        }

        keywordPtr++;
    }

    //Fill the rest of table with the alphabet
    const char *alphabetPtr = Alphabet;
    while (letterCopied != 25)
    {    
        int i = 0;
        for(; i < letterCopied; i++){
            if(table[i] == *alphabetPtr)
                break;
        }

        if(i == letterCopied){
            table[i] = *alphabetPtr;
            letterCopied++;
        }

        alphabetPtr++;
    }   

    //Create lookup table for the position of the character for better speed
    char lookup[26];
    for(int i = 0; i < 25; i++){
        lookup[table[i] - 65] = i;
    }


    //Pair of data
    char data1;
    char data2;

    PlayfairState state;
    state.done = false;
    state.input = input;
    state.skipOne = false;
    state.toAddDest = 0;
    state.toAddInput = 0;

    while(generate_chunk(&state, &data1, &data2)) {
        if(state.skipOne){
            state.input += 1;
            continue;
        }
        
        //Replace J with I because our table doesn't contains J
        if(data1 == 'J')
            data1 = 'I';
        if(data2 == 'J')
            data1 = 'I';

        int data1X = lookup[data1 - 65] % 5;
        int data2X = lookup[data2 - 65] % 5;
        int data1Y = lookup[data1 - 65] / 5;
        int data2Y = lookup[data2 - 65] / 5;

        //Check if character are in the same row
        if(data1Y == data2Y){
            data1X = (data1X + 1) % 5;
            data2X = (data2X + 1) % 5;

            dest[0] = table[data1X + (data1Y * 5)];
            dest[1] = table[data2X + (data2Y * 5)];
        }
        //Check if character are in the same column
        else if(data1X == data2X){
            data1Y = (data1Y + 1) % 5;
            data2Y = (data2Y + 1) % 5;

            dest[0] = table[data1X + (data1Y * 5)];
            dest[1] = table[data2X + (data2Y * 5)];
        } 
        //If all the above cases are false,
        //The characters from a rectangle
        else{ 
            int temp = data1Y;
            data1Y = data2Y;
            data2Y = temp;

            dest[1] = table[data1X + (data1Y * 5)];
            dest[0] = table[data2X + (data2Y * 5)];
        }
        
        dest += state.toAddDest;
        state.input += state.toAddInput;
    }

    dest[0] = 0; //Append a zero as end of string
}

/*
 * Encrypts a by playfair encrypted string
 * For more information about the algorithm's behavior, see the comment before encrypt
 */
void playfair_decrypt(const char *input, char *dest, const char *keyword){
    char table[25];
    memset(table, 0, 25);
    size_t keywordLen = strlen(keyword);

    int letterCopied = 0;
    int keywordPtr = 0;
    // Copy keyword into table
    while (letterCopied != 25)
    {  
    
        if (keyword[keywordPtr] == 0)
            break;

        //Check if keyword is J and add I if not so
        if(keyword[keywordPtr] == 'J'){
            int j = 0;
            for (; j < keywordPtr; j++)
            {
                if (table[keywordPtr] == 'I')
                    break;
            }

            if (j == keywordPtr)
            {
                table[letterCopied] = 'I';
                letterCopied++;
                keywordPtr++;
                continue;
            }
        }

        //Check if letter is alreay in table
        int j = 0;
        for (; j < keywordPtr; j++)
        {
            if (table[j] == keyword[keywordPtr])
                break;
        }

        if (j == keywordPtr)
        {
            table[letterCopied] = keyword[keywordPtr];
            letterCopied++;
        }

        keywordPtr++;
    }

    //Fill the rest of table with the alphabet
    const char *alphabetPtr = Alphabet;
    while (letterCopied != 25)
    {    
        int i = 0;
        for(; i < letterCopied; i++){
            if(table[i] == *alphabetPtr)
                break;
        }

        if(i == letterCopied){
            table[i] = *alphabetPtr;
            letterCopied++;
        }

        alphabetPtr++;
    }   

    //Create lookup table for the position of the character for better speed
    char lookup[26];
    for(int i = 0; i < 25; i++){
        lookup[table[i] - 65] = i;
    }


    char data1;
    char data2;
    while (input[0] != 0)
    {
        data1 = input[0];
        data2 = input[1];

        int data1X = lookup[data1 - 65] % 5;
        int data2X = lookup[data2 - 65] % 5;
        int data1Y = lookup[data1 - 65] / 5;
        int data2Y = lookup[data2 - 65] / 5;
        
        //Check if character are in the same row
        if(data1Y == data2Y){
            data1X = (data1X - 1) % 5;
            data2X = (data2X - 1) % 5;

            dest[0] = table[data1X + (data1Y * 5)];
            dest[1] = table[data2X + (data2Y * 5)];
        }
        //Check if character are in the same column
        else if(data1X == data2X){
            data1Y = (data1Y - 1) % 5;
            data2Y = (data2Y - 1) % 5;

            dest[0] = table[data1X + (data1Y * 5)];
            dest[1] = table[data2X + (data2Y * 5)];
        }
        else{
            int temp = data1Y;
            data1Y = data2Y;
            data2Y = temp;

            dest[1] = table[data1X + (data1Y * 5)];
            dest[0] = table[data2X + (data2Y * 5)];
        }

        input += 2;
        dest += 2;
    }

    dest[0] = 0;
}
