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
int GetFileName(char * full_name, char * name) {
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
    return 1;
}


/*
This function is used to read a file name 'name', and get its size and contents.
It returns the nb of objects read. If < 1, then there is an error.
donnée : name
résultat : size, contents
/!\ contents is stored in an allocated space. Which means it MUST be freed as soon as possible.
*/
int ReadFile(char * name, char ** content, int * size) {
	printf("\n>>>>> DEBUG >>>>> \t READFILE (debut)\n"); 

    // Definitions
	FILE * f;
    int nb_objects;

	// opening file
    //printf("\n>>>>> DEBUG >>>>> \t opening file \n");
	f = fopen(name, "rb");
	if (f == NULL) {
		perror("Erreur d'ouverture\n");
		exit(2);
	};

	// searching for file size : position at the end, get size, reposition at the start.
    //printf("\n>>>>> DEBUG >>>>> \t searching for size \n");
	if (fseek(f, 0, SEEK_END) != 0) {
		perror("Erreur de positionnement en fin\n");
		exit(3);
	};
	* size = (int) ftell(f);
    if (fseek(f, 0, SEEK_SET) != 0) {
		perror("Erreur de positionnement en début\n");
		exit(4);
	};

	// Creating buffer with correct size
    //printf("\n>>>>> DEBUG >>>>> \t linking content to correct memory allocation\n");
	*content = malloc(*size +1);
    if (*content == NULL) {
        perror("Erreur d'allocation de mémoire\n");
		exit(5);
    }

	// Storing in buffer file (no checks if reads correct nb of objects)
    //printf("\n>>>>> DEBUG >>>>> \t READFILE (reading file) size %d\n", *size);
	nb_objects = fread(*content, *size, 1, f);
    if (nb_objects < 1) {
        perror("Erreur de lecture dans le flot\n");
		exit(6);
    };

    // Closing stream
    // printf("\n>>>>> DEBUG >>>>> \tREADFILE (closing file)\n");
    if (fclose(f) == EOF) {
		perror("Erreur de fermeture du flot\n");
		exit(7);
	};
    
    // All is well
    printf(">>>>> DEBUG >>>>> \t READFILE (fin)\n");
    return 1;

}

/*
This function is used to write content of size content_size in a file of name name.
Each input parameter is a donnée.
Return : 1 if all is ok. Error code if not.
*/
int WriteFile(char * name, int content_size, char * content) {
    printf(">>>>> DEBUG >>>>> \t WRITEFILE (debut)\n"); 

    // Definitions
	FILE * f;
    int nb_bytes;

	// Opening file in mode w. Warning, this destroys any existent file with the same name.
	f = fopen(name, "wb");
	if (f == NULL) {
		perror("Erreur d'ouverture\n");
		exit(2);
	};

	// Writing in file
    // int fwrite(void *t, int size, int nbel, FILE *fp);
	nb_bytes = fwrite(content, 1, content_size, f);
    if (nb_bytes < content_size) {
        perror("Erreur d'écriture dans le flot\n");
		exit(8);
    };

    // Closing stream
    if (fclose(f) == EOF) {
		perror("Erreur de fermeture du flot\n");
		exit(7);
	};

    // all is well
    printf(">>>>> DEBUG >>>>> \t WRITEFILE (fin)\n");
    return 1;

}


/*
This function takes an error code, prints associated message, and exit if asked.
*/
void CheckError(int error, int quit) {
    switch(error) {
		case 1 :
		    printf("No errors to report.\n");
		    break;
		case 2 :
			printf("Erreur d'ouverture.\n");
			break;
		case '3' :
			printf("Erreur de positionnement en fin.\n");
			break;
		case '4' :
			printf("Erreur de positionnement en début.\n");
			break;
		case '5' :
			printf("Erreur d'allocation de mémoire.\n");
			break;
		case '6' :
			printf("Erreur de lecture dans le flot.\n");
			break;
		case '7' :
			printf("Erreur de fermeture du flot.\n");
			break;
		case '8' :
			printf("Erreur d'écriture dans le flot.\n");
			break;
		default :
			printf("Not yet registered error.\n");
			break;
	};
    if (quit) {
        exit(EXIT_FAILURE);
    };
}


/*
This function is use to get the output of a system command in output[]
For simplicity, we have decided to set the size of the output. 
There are several way of getting the ouput of a command : 
- either storing ouput in file and reading file (system("ls * > file 2>&1")) but that at least
requires having a file name that does not already exist.
- or creating a pipe between the calling program and the excuted command, where we can both write and
read. /!\ we are creating a child process : it must be closed.
*/
int GetFileSystem (char * output, const int size, const char * command) {
    // Definitions
    FILE * f;
    
    // Open new flow
    if ((f = popen(command, "r")) == NULL) {
		perror("Erreur d'ouverture\n");
		exit(2);
	};

    // storing command response in file_system
    fread(output, 1, size, f);
	//printf("output : \n %s\n", output);
    
    // Closing flow
    pclose(f);

    // All is well
    return 1;

}

