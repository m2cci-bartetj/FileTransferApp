/* This script is used to manupulate files

* reading_file(char * fname) : reads the contents of file_name and stores it.
Since we use a dyamic allocation of the memory, the function must clean after usage. 
Which means usage of the file content must be done inside the function.
We could divide this function in two, with a part creating/freeing, but that would introduce
possible memory leaks if one forgot to clean up afterwards. Therefore usage will
be done in the function.

* long int writing_file(char * file_name, long int content_size, char * content) : writes content into
file_name (destroys it if exists). Inserting the content size is not necessary (we could use strlen),
but provides the usual redundancy which is found in the fwrite function.
This time, there is no allocation of memory outside the stream.
Returns : nb of actual bytes written. If < content_size, then we've had a problem.

*/

#include "test.h"       /* pour ces propres fonctions & definitions & inclusions*/
#include "fon.h"        /* pour les primitives socket */

int main() {
    // Calls for full file functions
    char * file_name_r = "README";
    char * file_name_w = "README2";
    char * content = "I'm typing anew ?\n";
    long int size = strlen(content);
    long int size_w;

    reading_file(file_name_r);
    size_w = writing_file(file_name_w, size , content);
    printf("input size : %ld\n", size);
    printf("output size : %ld\n", size_w);
    reading_file(file_name_w);

}

int reading_file(char * file_name) {
    // Definitions
	long int file_size;
	FILE * f;

	// opening file
	f = fopen(file_name, "rb");
	if (f == NULL) {
		perror("Erreur d'ouverture");
		exit(1);
	};

	// searching for file size
	if (fseek(f, 0, SEEK_END) != 0) {
		perror("Erreur de positionnement en fin");
		exit(1);
	};
	file_size = ftell(f);
    if (fseek(f, 0, SEEK_SET) != 0) {
		perror("Erreur de positionnement en début");
		exit(1);
	};

	// Creating buffer with correct size
	char * file_content = malloc(file_size +1);
    if (file_content == NULL) {
        perror("Erreur d'allocation de mémoire");
		exit(1);
    }

	// Storing in buffer file
	fread(file_content, file_size, 1, f);
    file_content[file_size] = '\0';

    // Closing stream
    if (fclose(f) == EOF) {
		perror("Erreur de fermeture du flot");
		exit(1);
	};
    // Printing the file for check in both unix & our new string
    //const char * command = "cat README";
    //system(command);
    printf("%s\n", file_content);

    // Freeing the allocated memory
    free(file_content);

    // All is well
    return 1;

}

long int writing_file(char * file_name, long int content_size, char * content) {
    // Definitions
	FILE * f;
    long int nb_bytes;

	// Opening file in mode w. Warning, this destroys any existant file with the same name.
	f = fopen(file_name, "wb");
	if (f == NULL) {
		perror("Erreur d'ouverture");
		exit(1);
	};

	// writing in file
    // int fwrite(void *t, int size, int nbel, FILE *fp);
	nb_bytes = fwrite(content, 1, content_size, f);
    if (nb_bytes != content_size) {
        perror("Erreur d'écriture dans le flot");
		exit(1);
    };

    // Closing stream
    if (fclose(f) == EOF) {
		perror("Erreur de fermeture du flot");
		exit(1);
	};

    // return nb of bytes written
    return nb_bytes;

}