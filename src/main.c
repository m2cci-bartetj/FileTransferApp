#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define INT_SIZE 5

int main() {
    //definitions
    char file_size[INT_SIZE];				// file size in char format
	int size;								// file size in int format
    
    //core
    size = 816846;
    printf("size int : %d \n", size);
    printf("size hexa : %x \n", size);
    // int to char
    file_size[0] = size & 0xff;
    file_size[1] = (size >> 8) & 0xff;
    file_size[2] = (size >> 16) & 0xff;
    file_size[3] = (size >> 24) & 0xff;

    sprintf(file_size, "%x", size);
    printf("file_size : %s\n", file_size);

    size = (int)strtol(file_size, NULL, 16);
    printf("size transformed : %d \n", size);

    
    
    //for (int i = 0; i<4; i++) {
    //    printf("0x%x\n", file_size[i]);
    //};

    

    // char to int


}

/* We need to convert int to char but keeping the same bytes arrangement.
 We just need the code to recognise our bytes as chars even though they have been declared as int
 Solution : 
    - int size; char file_size[INT_SIZE]; file_size = (char *) &size; size = (int) * file_size. Unfortunately, this
     only keeps the first byte. Because of the endianess, these are the bit de poids faible, and that is why the convertion
     works for int < 127 (since we are in signed char). Too bad, that would have been neat.
    - seperate each byte of the int and store them in the char table. This is with && + >> right shift. This is what we will be doint here.
*/

