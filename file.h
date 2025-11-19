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
int IsConnected( int socket_number);
int GetFileName(char * full_name, char * name);
int FileExists( char * name) ;
int ReadFile(char * name, char ** content, int * size);
int WriteFile(char * name, int content_size, char * content) ;
void CheckError(int error, int quit);