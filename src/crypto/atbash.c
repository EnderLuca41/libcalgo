#include <ctype.h>

const static char Lookup[26] = {
    'Z' ,'Y' ,'X' ,'W' ,'V' ,
    'U' ,'T' ,'S' ,'R' ,'Q' ,
    'P' ,'O' ,'N' ,'M' ,'L' ,
    'K' ,'J' ,'I' ,'H' ,'G' ,
    'F' ,'E' ,'D' ,'C' ,'B' ,
    'A'
};

//Letters in key must be uppercase
void atbash_encrypt_decrypt(char *text){
    for(; *text != 0; text++){
        if(islower(*text)){
            *text = Lookup[*text - 97] + 32;
            continue;
        }

        if(isupper(*text)){
            *text = Lookup[*text - 65];
        }
    }
}
