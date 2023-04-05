#include <stdint.h>
#include <stdlib.h>
#include <string.h>


/* Returns 0 on success,
 * -1 if the size of the polybius table was bigger than alphabet,
 * -2 if if a character from key or data was not alphabet,
 * and -3 if the polybiusSize > 9
 */
int nihilist_cipher_encrypt(unsigned char *data, size_t dataLen,  
                            const unsigned char *key, const unsigned char *alphabet,
                             size_t alphabetLen, uint32_t polybiusSize){
    if(alphabetLen > polybiusSize * polybiusSize)
        return -1;
    if(polybiusSize > 9)
        return -3;

    //Get the polybius table values of the key
    size_t keyLen = strlen(key);
    char *keyPolybius = malloc(keyLen);
    for(size_t i = 0; i < keyLen; i++){

        //Find index in alphabet
        size_t j = 0;
        for(; j < alphabetLen; j++){
            if(alphabet[j] == key[i])
                break;
        }

        //Check if current char was not in alphabet
        if(j == alphabetLen){
            free(keyPolybius);
            return -2;   
        }

        //Calculate polybius position
        uint32_t polybiusValue = (((j / polybiusSize) * 10) + 10) + ((j % polybiusSize) + 1);
        keyPolybius[i] = polybiusValue;
    }

    //Iterate over the data and encrypt it with the key values and the polybius table
    for(size_t i = 0; i < dataLen; i++){

        //Find index in alphabet
        size_t j = 0;
        for(; j < alphabetLen; j++){
            if(alphabet[j] == data[i])
                break;
        }

        //Check if current char was not in alphabet
        if(j == alphabetLen){
            free(keyPolybius);
            return -2;   
        }

        //Calculate polybius position
        uint32_t polybiusValue = (((j / polybiusSize) * 10) + 10) + ((j % polybiusSize) + 1);
        data[i] = polybiusValue + keyPolybius[i % keyLen];
    }

    free(keyPolybius);
    return 0;
}

/* Returns 0 on success,
 * -1 if the size of the polybius table was bigger than alphabet,
 * -2 if if a character from key was not in alphabet,
 * and -3 if the polybiusSize > 9
 */
int nihilist_cipher_decrypt(unsigned char *data, size_t dataLen, 
                            const unsigned char *key, const unsigned char *alphabet, 
                            size_t alphabetLen,uint32_t polybiusSize){
    if(alphabetLen > polybiusSize * polybiusSize)
        return -1;
    if(polybiusSize > 9)
        return -3;

    //Get the polybius table values of the key
    size_t keyLen = strlen(key);
    char *keyPolybius = malloc(keyLen);
    for(size_t i = 0; i < keyLen; i++){

        //Find index in alphabet
        size_t j = 0;
        for(; j < alphabetLen; j++){
            if(alphabet[j] == key[i])
                break;
        }

        //Check if current char was not in alphabet
        if(j == alphabetLen){
            free(keyPolybius);
            return -2;   
        }

        //Calculate polybius position
        uint32_t polybiusValue = (((j / polybiusSize) * 10) + 10) + ((j % polybiusSize) + 1);
        keyPolybius[i] = polybiusValue;
    }

    //Iterate over the data and decrypt it
    for(size_t i = 0; i < dataLen; i++){
        data[i] -= keyPolybius[i % keyLen];

        uint32_t x = data[i] % 10 - 1;
        uint32_t y = data[i] / 10 - 1;
    
        uint32_t pos = y * polybiusSize + x;
        data[i] = alphabet[pos];
    }

    free(keyPolybius);
    return 0;
}