/**************************** INCLUSIONS ****************************/
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <netdb.h>

/**************************** DEFINITIONS  ****************************/



/***************************** FONCTIONS  *****************************/
int GetFileName(char * full_name, char * name);
int GetFileSize(char * name, int * size);
int ReadFile(char * name, char * content, int size);
int WriteFile(char * name, int content_size, char * content);
int GetFileSystem (char * output, const int size, const char * command);