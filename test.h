/**************************** INCLUSIONS ****************************/
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <netdb.h>

/**************************** DEFINITIONS  ****************************/



/***************************** FONCTIONS  *****************************/

int reading_file(char * fname); 
long int writing_file(char * file_name, long int content_size, char * content);