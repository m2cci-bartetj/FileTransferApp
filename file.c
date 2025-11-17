/* This script is used for all functions needed by both client and server
*/

#include "file.h"       /* pour ces propres fonctions & definitions & inclusions*/
#include "fon.h"        /* pour les primitives socket */

/*
This function returns :
    - 1 if the connection still exists for socket id socketNumber
    - 0 if not
In order to know whether or not a connection still exists or not, several options : 
    1. Look into netstat -... for the correct socket id and get the status. If status = ESTABLISHED
        then return 1. Else return 0. This requires system( bash_command ) command. But that command takes only
        a constant bash_command. And here the socket id is a variable. So we need a bash script called with
        arguments = socketNumber
    2.  Look into the files that netstat looks into to get the connection status. Use read_file(), and add
        a search in file function. OR again use a bash script with grep command on the correct file, and 
        either (1) store the output information in file that will need to be read; or (2) get the ouput with
        stdio stdout flow. No idea how to at the moment.
    3. Suppose that the client sends a warning before close. Do a socket_read. If you get the warning, then return
    0, if not, return 1. Attention, h_reads est blocant ! C'est à dire que si il ne reçoit rien, il attends indéfiniement !
    Donc il faut envoyer un message disant que l'on veut poursuivre la connection. Autre chose : il faut s'assurer
    que l'on a bien lu tout ce qu'il y avait dans le buffer avant !
    4. Get the information out of socket status. No idea if possible, but surely should be right ?
Note : only the client can close a connection.
Note : if we use bash scripts or store information for the connection in a file, we must be sure that this file
does not already exists = does not belong to the original 'useful' file system.
*/
int IsConnected(int socket_number) {
    return 1;
}



/*
This function returns : 
    - 0 if file_name does not exist
    - 1 if file_name does
Note that file_name may contain a path. Ex : '../../DOC/README'.
Note that depending on the OS, the '/' between the repertories are sometimes '\'.
Solution : 
    1. try opening this file. If works, return 1, else return 0. Not great since we could have an error opening an existing file
    2. use bash command. Again, not great since this command would be 'ls file_name', which is a variable. So system() does not
     work => we need bash script with ouput arguments. Store result either in file to be read and destroyed, or in stdout.
    3. try accessing to file system via C command. No idea if possible.
*/
int FileExists(char * name) {
    return 1;
}


/*
This function is used to seperate the path from the actual file name.
Indeed, since we do not specify WHERE we put the file in the server, we must only
give the file name without the path to the server.

donnée : full_file_name : with path. Ex '../../DOC/README'
résultat : file_name : without path. Ex : 'README'
/!\ Depending on the OS, we have backslashes '\'.
*/
void GetFileName(char * full_name, char * name) {
    printf("\n>>>>> DEBUG >>>>> \t GETFILENAME (debut)\n"); 

    // Definition
    int i, j, max;

    // Get position of last '/' or '\'.
    i = strlen(full_name);
    while ( i >= 0 && (full_name[i] != '/' && full_name[i] != '\\')) {
        i --;
    };

    // copy full_name[i+1 -> strlen] into name[0 -> strlen -i-1]. Iclude \0.
    max = ((int) strlen(full_name) -i -1);
    for (j = 0; j <= max; j++) {
        name[j] = full_name[i+1];
        i++;
    }

    // print to check
    printf("First name : %s\n", full_name);
    printf("Final name : %s\n", name);
    //printf("length final name : %ld\n", strlen(name));
    printf("\n>>>>> DEBUG >>>>> \t GETFILENAME (fin)\n");

    // Maybe a return to check that all is ok would be nice ?
}


/*
This function is useed to read a file name 'name', and get its size and contents.
It returns the file contents
donnée : name
résultat : size
ouput : contents
/!\ contents is stored in an allocated space. Which means it MUST be freed as soon as possible.
/!\ we cannot pass contents as a parameter of this fonction. Indeed: 
- char s[] = "string" results in a modifiable array of 7 bytes
- char *s = "string" results in a pointer to some read-only memory containing the string-literal "string".
WHICH means that if you want to modify the contents of your string, the first is the way to go.
BUT we do not know the size of contents (nor the contents), so we cannot initialize it outside of the function. Thus it cannot be passed
as a parameter.
*/
int ReadFile(char * name, char ** content, int * size) {
	printf("\n\n>>>>> DEBUG >>>>> \t READFILE (debut)\n"); 

    // Definitions
	FILE * f;

	// opening file
    //printf("\n>>>>> DEBUG >>>>> \t opening file \n");
	f = fopen(name, "rb");
	if (f == NULL) {
		perror("Erreur d'ouverture");
		exit(1);
	};

	// searching for file size : position at the end, get size, reposition at the start.
    //printf("\n>>>>> DEBUG >>>>> \t searching for size \n");
	if (fseek(f, 0, SEEK_END) != 0) {
		perror("Erreur de positionnement en fin");
		exit(1);
	};
	* size = (int) ftell(f);
    if (fseek(f, 0, SEEK_SET) != 0) {
		perror("Erreur de positionnement en début");
		exit(1);
	};

	// Creating buffer with correct size
    //printf("\n>>>>> DEBUG >>>>> \t linking content to correct memory allocation\n");
	*content = malloc(*size +1);
    if (*content == NULL) {
        perror("Erreur d'allocation de mémoire");
		exit(1);
    }

	// Storing in buffer file (no checks if reads correct nb of objects)
    //printf("\n>>>>> DEBUG >>>>> \t READFILE (reading file) size %d\n", *size);
	fread(*content, *size, 1, f);
    printf("\n>>>>> DEBUG >>>>> \t READFILE (reading file) content \n%s\n", *content);
    printf("\n>>>>> DEBUG >>>>> \t READFILE (reading file) length content %ld\n", strlen(*content));

    // Closing stream
    // printf("\n>>>>> DEBUG >>>>> \tREADFILE (closing file)\n");
    if (fclose(f) == EOF) {
		perror("Erreur de fermeture du flot");
		exit(1);
	};
    
    // All is well
    printf("\n>>>>> DEBUG >>>>> \t READFILE (fin)\n");
    return 1;

}


