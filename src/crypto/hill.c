#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
static const char StandardAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz !?/,;.:-_+*#&";

static int mod(int x,int N){
    return (x % N + N) %N;
}

//Find next character that is in the given lookup table
static const char *find_char(const char *input, const int *lookup){
    while(*input != 0){ 
        if(lookup[*input] != -1){ //lookup[i] is -1 if the character was not part of the passed alphabet
            return input;
        }
        input++;
    }

    return input;
}

static void matrix_mult_encrypt(char *dest, const int *block, const char *key, 
                        const int lookup[255], int tableSize, int alphabetSize){
    for(int i = 0; i < tableSize; i++){
        int result = 0;

        for(int j = 0; j < tableSize; j++){
            result += lookup[key[i * tableSize + j]] * block[j];
        }
        
        dest[i] = result % alphabetSize;
    }
}

/*
 * These two seperate function exist because in the decryption process,
 * the characters are already exchanged to their index in the alphabet before
 * the key is in inversed
 */

static void matrix_mult_decrypt(char *dest, const int *block, const char *inverseKey, int tableSize, int alphabetSize){
    for(int i = 0; i < tableSize; i++){
        int result = 0;

        for(int j = 0; j < tableSize; j++){
            result += ((int) inverseKey[i * tableSize + j]) * block[j];    
        }

        dest[i] = mod(result, alphabetSize);
    }
}


void matrix_copy(const int *src, int *dest, int columnExclude, int size){
    for(int i = 1; i < size; i++){
        int column = 0;

        for(int j = 0; j < size; j++){
            if(j == columnExclude)
                continue;
            dest[(i-1) * (size-1) + column] = src[i * size + j];
            column++;
        }
    }
}

int determinant(const int matrix[], int size){
    if(size == 3){
        return (matrix[0] * matrix[4] * matrix[8]) +
               (matrix[1] * matrix[5] * matrix[6]) +
               (matrix[2] * matrix[3] * matrix[7]) -
               (matrix[2] * matrix[4] * matrix[6]) -
               (matrix[1] * matrix[3] * matrix[8]) -
               (matrix[0] * matrix[5] * matrix[7]);
    }

    if(size == 2){
        return (matrix[0] * matrix[3] - matrix[1] * matrix[2]);
    }

    int *buffer = malloc((size-1)*(size-1) * sizeof(int));
    int sign = 1;
    int det = 0;
    for(int i = 0; i < size; i++){
        matrix_copy(matrix, buffer, i, size);
        det += sign * matrix[i] * determinant(buffer, size-1);
        sign = -sign;
    }

    free(buffer);
    return det;
}

void matrix_of_minors(const int input[], int dest[], int size){
    int *detBuffer = malloc((size-1) * (size-1) * sizeof(int));

    int detBufferCounter = 0;
    int columExclude;
    int rowExclude;
    for(int i = 0; i < size*size; i++){
        columExclude = i / size;
        rowExclude = i % size;

        for(int y = 0; y < size; y++){
            if(y == rowExclude)
                continue;
            for(int x = 0; x < size; x++){
                if(x == columExclude)
                    continue;
                detBuffer[detBufferCounter] = input[y * size + x];
                detBufferCounter++;
            }
        }


        *dest = determinant(detBuffer, size-1);
        detBufferCounter = 0;
        dest++;
    }

    free(detBuffer);
}

void cofactor_matrix(int matrix[], int size){
    if(size % 2 == 1){
        int sign = 1;
        for(int i = 0; i < size*size; i++){
            matrix[i] = sign * matrix[i];
            sign = -sign;
        }
    }
    else{
        int sign = 1;
        for(int i = 0; i < size; i++){
            sign = -sign;
            for(int j = 0; j < size; j++){
                sign = -sign;
                matrix[i * size + j] = sign * matrix[i * size + j];
            } 
        }
    }
}


//Returns false if the matrix was not inversiable
bool inverse_matrix(const int matrix[], char dest[], int size, int alphabetSize){
    int *matrixBuffer = malloc(size * size * sizeof(int)); 
    if(size == 2){
        matrixBuffer[0] = matrix[3];
        matrixBuffer[3] = matrix[0];
        matrixBuffer[1] = -matrix[1];
        matrixBuffer[2] = -matrix[2];
    }
    else{
        matrix_of_minors(matrix, matrixBuffer, size);
        cofactor_matrix(matrixBuffer, size);
    }

    int det = mod(determinant(matrix, size), alphabetSize);
    int inveserseDet = 0;
    //Bruteforce the inverse Determinant
    for (int i = 0; true; i++){
        
        if(mod(det * i, alphabetSize) == 1 % alphabetSize){
            inveserseDet = i;
            break;
        }

        if(i == 10000){
            //There is no inverseDeterminant
            free(matrixBuffer);
            return false;
        }
    }

    for(int i = 0; i < size*size; i++){
        matrixBuffer[i] = mod(matrixBuffer[i], alphabetSize);
        matrixBuffer[i] *= inveserseDet;
        matrixBuffer[i] = mod(matrixBuffer[i], alphabetSize);
        dest[i] = (char) matrixBuffer[i];
    }

    free(matrixBuffer);
    return true;
}

bool hill_encrypt(const char *input, char *dest, const char *key, const char *alphabet){
    if(alphabet == NULL)
        alphabet = StandardAlphabet;

    //Check if the length of key has a non floating point square root
    //This is because it is later represented as n * n big table
    size_t keyLen = strlen(key);
    double squared = sqrt((double) keyLen);
    if(squared != floor(squared))
        return false;
    
    int alphabetLen = strlen(alphabet);

    //Create a loopup table for alphabet, 
    //so we can the index in the alphabet without traversing it everytime
    int lookup[255];
    memset(lookup, 0xFF, 255 * sizeof(int)); //Set every value to -1
    for(unsigned int i = 0; i < alphabetLen; i++){
        lookup[(unsigned char) alphabet[i]] = i;
    }
    
    int tableSize = (int) squared;
    int *block = malloc(tableSize * sizeof(int));
    bool dataLeft = true;
    const char *initDest = dest;
    
    while(dataLeft){    
        if(input[0] == 0){
            break;
        }
        for(int i = 0; i < tableSize; i++) {
            const char *nextChar = find_char(input, lookup);
            
            if(*nextChar == 0){
                //If there not enough characters left in input to fill the last block
                //It gets padded with the first character of alphabet
                for(int j = i; j < tableSize; j++){
                    block[j] = lookup[alphabet[0]];
                }
                dataLeft = false;
                break;
            }

            block[i] = lookup[*nextChar];
            input = nextChar + 1;
        }
        
        matrix_mult_encrypt(dest, block, key, lookup, tableSize, alphabetLen);

        //Convert number in alphabet to actual character
        for(int i = 0; i < tableSize; i++){
            dest[i] = alphabet[dest[i]];
        }
        dest += tableSize;
    }

    dest[0] = 0; //Append a ending 0, so it is a valid C-String

    free(block);
    return (dest) - (initDest);
}

bool hill_decrypt(const char *input, char *dest, const char *key, const char *alphabet){
    if(alphabet == NULL)
        alphabet = StandardAlphabet;

    //Check if the length of key has a non floating point square root
    //This is because it is later represented as n * n big table
    size_t keyLen = strlen(key);
    double squared = sqrt((double) keyLen);
    if(squared != floor(squared))
        return false;
    
    int alphabetLen = strlen(alphabet);


    //Create a loopup table for alphabet, 
    //so we can the index in the alphabet without traversing it everytime
    int lookup[255];
    memset(lookup, 0xFF, 255 * sizeof(int)); //Set every value to -1
    for(unsigned int i = 0; i < alphabetLen; i++){
        lookup[(unsigned char) alphabet[i]] = i;
    }

    int tableSize = (int) squared;
    int *block = malloc(tableSize * sizeof(int));
    char *inverseKey = malloc(keyLen * sizeof(char));
    int *keyInt  = malloc(keyLen * sizeof(int));
    
    //We need the key in as int representation for the inverse
    for(int i = 0; i < keyLen; i++)
        keyInt[i] = lookup[key[i]]; 

    bool inversiable = inverse_matrix(keyInt, inverseKey, tableSize, alphabetLen);
    if(inversiable == false)
    {
        free(block);
        free(inverseKey);
        free(keyInt);    
        return false;    
    }

    while(*input != 0){
        //Copy the alphabet index of the characters
        for(int i = 0; i < tableSize; i++){
            block[i] = lookup[input[i]];
        }

        matrix_mult_decrypt(dest, block, inverseKey, tableSize, alphabetLen);

        //Convert the alphabet indecis to the character 
        for(int i = 0; i < tableSize; i++){
            dest[i] = alphabet[dest[i]];
        }

        input += tableSize;
        dest += tableSize;
    }
    dest[0] = 0; //Append a ending 0, so it is a valid C-String

    free(keyInt);
    free(inverseKey);
    free(block);
    return true;
}