#include <ctype.h>

/*
 * Encrypts with the standard bacon substitution table (u = v and i = j) 
 * a and b are the bileterals used to encrypt the text
 */
void bacon_encrypt(const char *text, char *dest, char a, char b){
    for(; *text != '\0'; text++){
        if(!isalpha(*text))
            continue;
        int num = toupper(*text) - 65;
        
        //Because we I=J and U=V we have our alphabet has only 24 characters
        //Depending on the character, we have to decrement its value 1-2 times
        if(num >= 9)
            num--;
        if(num >= 20)
            num--;

        //A basic decimal to binary conversion but instead of 1's and 0's we use the value of a and b
        int i = 4;
        while(i != -1){
            int rem = num % 2;
            num /= 2;
            if(rem)
                dest[i] = b;
            else
                dest[i] = a;
            i--;
        }

        dest[5] = ' ';
        dest += 6;
    }

    dest[0] = '\0';
}

void bacon_decrypt(const char *text, char *dest, char a, char b){
    for(;*text != '\0'; text += 6){

        //Basic binary to decimal conversion
        int num = 0;
        int pow = 1;
        for(int i = 4; i != -1; i--){
            num += (text[i] == b) * pow;
            pow *= 2;
        }

        //Add 1-2 because our alphabet has only 24 character
        if(num >= 8)
            num++;
        if(num >= 20)
            num++;

        *dest = num + 'A';
        dest++;
    }
    *dest = '\0';
}
