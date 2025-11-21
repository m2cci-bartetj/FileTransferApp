/* 
-----------------------------------------------------------------------------------------
This script is used for all additional functions needed by both client and server
-----------------------------------------------------------------------------------------
*/

#include "file.h"       /* pour ces propres fonctions & definitions & inclusions*/

/*
-----------------------------------------------------------------------------------------
                int GetFileName(char * full_name, char * name)
-----------------------------------------------------------------------------------------
This function is used to seperate the path from the actual file name.
Indeed, since we do not specify WHERE we put the file in the server, we must only
give the file name without the path to the server.

donnée : full_name : with path. Ex '../../DOC/README'
résultat : name : without path. Ex : 'README'
/!\ Depending on the OS, we can have backslashes '\'.
-----------------------------------------------------------------------------------------
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

    // All is well
    return 0;
}


/*
-----------------------------------------------------------------------------------------
            int GetFileSize(char * name, int * size)
-----------------------------------------------------------------------------------------
This function is used to get the size of a given file.
It returns the 0 if all is well, or errno if not.
donnée : name
résultat : size
Note : we could return the size +1 in order to add at the end of file_content the end mark '\0'.
It is quite un-necessary UNLESS one wants to print the file contents using printf("%s").
But this generally produces an error when opening with regular text editor the copied file,
that is seen as binary.
-----------------------------------------------------------------------------------------
*/
int GetFileSize(char * name, int * size) {
	printf("\n>>>>> DEBUG >>>>> \t GETFILESIZE (debut)\n"); 
    // Definitions
	FILE * f;

	// opening file
    //printf("\n>>>>> DEBUG >>>>> \t opening file \n");
	f = fopen(name, "rb");
	if (f == NULL) {
		perror("Erreur d'ouverture");
		return errno;
	};

	// searching for file size : position at the end, get size
    //printf("\n>>>>> DEBUG >>>>> \t searching for size \n");
	if (fseek(f, 0, SEEK_END) != 0) {
		perror("Erreur de positionnement en fin");
		return errno;
	};
	//* size = (int) ftell(f) +1 ;
    * size = (int) ftell(f) ;
    
    // All is well
    printf(">>>>> DEBUG >>>>> \t GETFILESIZE (fin)\n");
    return 0;

}

/*
-----------------------------------------------------------------------------------------
            int ReadFile(char * name, char ** content, int size)
-----------------------------------------------------------------------------------------
This function is used to read a file name 'name' of given size, and get its contents.
It returns 0 if there are not errors. Else it retruns errno.
donnée : name, size
résultat : contents
/!\ contents is stored in an allocated space. Which means it MUST be freed as soon as possible.
Note : We cannot read file contents with a printf. Printf("%s", file_content) reads until mark '\0'.
If it is not here, it continues to read beyond the variable into to non-allocated memory !
If we usze size +1 and put it there, the copied filed is seen as binary and it not quite the
same one as the original (\0 added at the end.)
-----------------------------------------------------------------------------------------
*/
int ReadFile(char * name, char * content, int size) {
	printf("\n>>>>> DEBUG >>>>> \t READFILE (debut)\n"); 

    // Definitions
	FILE * f;
    int nb_objects;

	// opening file
    //printf("\n>>>>> DEBUG >>>>> \t opening file \n");
	f = fopen(name, "rb");
	if (f == NULL) {
		perror("Erreur d'ouverture");
		return errno;
	};

	// Storing in buffer file 
    //printf("\n>>>>> DEBUG >>>>> \t READFILE (reading file) size %d\n", *size);
	nb_objects = fread(content, size, 1, f);
    if (nb_objects < 1) {
        perror("Erreur de lecture dans le flot");
		return errno;
    };

    // adding the end mark for useful printing only.
    //content[size] = '\0';

    // Closing stream
    // printf("\n>>>>> DEBUG >>>>> \tREADFILE (closing file)\n");
    if (fclose(f) == EOF) {
		perror("Erreur de fermeture du flot");
		return errno;
	};
    
    // All is well
    printf(">>>>> DEBUG >>>>> \t READFILE (fin)\n");
    return 0;

}

/*
-----------------------------------------------------------------------------------------
        int WriteFile(char * name, int content_size, char * content)
-----------------------------------------------------------------------------------------
This function is used to write content of size content_size in a file of name name.
Each input parameter is a donnée.
Return : 0 if all is ok. Error code if not.
-----------------------------------------------------------------------------------------
*/
int WriteFile(char * name, int content_size, char * content) {
    printf(">>>>> DEBUG >>>>> \t WRITEFILE (debut)\n"); 

    // Definitions
	FILE * f;
    int nb_bytes;

	// Opening file in mode w. Warning, this destroys any existent file with the same name.
	f = fopen(name, "wb");
	if (f == NULL) {
		perror("Erreur d'ouverture");
		return errno;
	};

	// Writing in file
    // int fwrite(void *t, int size, int nbel, FILE *fp);
	nb_bytes = fwrite(content, 1, content_size, f);
    if (nb_bytes < content_size) {
        perror("Erreur d'écriture dans le flot");
		return errno;
    };

    // Closing stream
    if (fclose(f) == EOF) {
		perror("Erreur de fermeture du flot");
		return errno;
	};

    // all is well
    printf(">>>>> DEBUG >>>>> \t WRITEFILE (fin)\n");
    return 0;

}


/*
-----------------------------------------------------------------------------------------
          int GetFileSystem (char * output, const int size, const char * command) 
-----------------------------------------------------------------------------------------
This function is use to get the output of a system command in output[]
For simplicity, we have decided to set the size of the output. 
There are several way of getting the ouput of a command : 
- either storing ouput in file and reading file (system("ls * > file 2>&1")) but that at least
requires having a file name that does not already exist.
- or creating a pipe between the calling program and the excuted command, where we can both write and
read. /!\ we are creating a child process : it must be closed.
-----------------------------------------------------------------------------------------
*/
int GetFileSystem (char * output, const int size, const char * command) {
    // Definitions
    FILE * f;
    
    // Open new flow
    if ((f = popen(command, "r")) == NULL) {
		perror("Erreur d'ouverture de processe");
		return errno;
	};

    // storing command response in file_system
    fread(output, 1, size, f);
	//printf("output : \n %s\n", output);
    
    // Closing flow
    if (pclose(f) == -1) {
        perror("Erreur de fermeture du flot");
		return errno;
    };

    // All is well
    return 0;

}

