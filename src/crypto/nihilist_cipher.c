#include <stdint.h>
#include <stdlib.h>
#include <string.h>


/* Returns 0 on success,
 * -1 if the size of the polybius table was bigger than alphabet
 * and -2 if if a character from key or data was not alphabet
 */
int nihilist_cipher(unsigned char *data, const unsigned char *key, const unsigned char *alphabet,  uint32_t polybiusSize){
    size_t alphabetLen = strlen(alphabet);
    if(alphabetLen > polybiusSize * polybiusSize)
        return -1;

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
    for(size_t i = 0; data[i] != 0; i++){

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