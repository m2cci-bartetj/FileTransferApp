/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/	

#include<stdio.h>
#include <curses.h>

#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <errno.h>

#include "fon.h"     		/* Primitives de la boite a outils */
#include "file.h"	    	/* primitives pour la manipulation de fichiers*/

// Mon Port serveur
#define SERVICE_DEFAUT "1111"
#define MAX_NAME 100		// FILENAME_MAX of 4096 creates pb with accept : memory shortage ?
#define INT_SIZE 5
#define MAX_FILESYSTEM_SIZE 500

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}


/******************************************************************************/
/* Procedure correspondant au traitemnt du serveur de votre application */
void serveur_appli(char *service)
{
	/* 0. DEFINTIONS */ 
	int id_socket;
	int id_new_socket;
	int nb_req_att = 5;
	struct sockaddr_in *p_adr_serv;
	struct sockaddr_in p_adr_client;

	char file_system[MAX_FILESYSTEM_SIZE];	// file_system
	char file_name[MAX_NAME];				// Name of the file
	char file_size[INT_SIZE];				// file size in char format
	char error[INT_SIZE];					// Error handling in char format
	char *file_content;						// pointer for the content of the file. Allocated dynamically => must be freed
	char choice ;							// choice of the procedure to follow
	int nb_bytes_written, nb_bytes_read;	// Mostly used for visual error handling
	int error_code_serv, error_code_clie;	// Error handling in int format
	int size;								// file size in int format

	/* 1. CONFIGURATION D'UNE CONNEXION*/
	// 1.1. Création d'une socket
	id_socket = h_socket(AF_INET, SOCK_STREAM);
	if (id_socket < 0) {
		printf ( "\nERREUR 'h_socket' : Creation socket impossible \n" );
		exit(1);
	};

	// 1.2. Paramétrage de la socket. Récupération de l'adresse de la socket, IP, Port
	adr_socket(service, NULL, SOCK_STREAM, &p_adr_serv);
	h_bind(id_socket, p_adr_serv);
	// /!\ TEST A FAIRE : renvoie void donc pas de possibilité de tester

	// 1.3. Mise en écoute de connexion
	h_listen( id_socket, nb_req_att );
	// /!\ TEST A FAIRE: renvoie void donc pas de possibilité de tester
	// si cette fonction ne marche pas, alors on a une boucle infini
	// qui s'exécute sans s'arrêter. Mettre sleep(5) permet au moins
	// d'avoir le temps de forcer l'arrêt du pgm en cas de pb.
	//sleep(5);

	// 1.4 boucle infinie d'accpetation et de fermeture de connexion
	while (1) {
		/* 2. ACCEPTATION D'UNE CONNEXION*/
		id_new_socket = h_accept(id_socket, &p_adr_client);
		if (id_socket < 0) {
			printf ( "\nERREUR 'h_accept' : Acceptation impossible \n" );
			exit(1);
		};

		/* 3. ECHANGE DE DONNÉES*/
		/* while socket status = Established, continuer. Sinon, cela veut dire que le client a 
		envoyé un close => il faut donc s'arrêter. Lorsque le client s'arrête, il envoie char '3'.
		A noter que cet échange n'est posible que si tout c'est bien passé au-dessu (ce qui n'est pas
		testé pour le moment)*/
		
		choice = '0';
		while ( choice != '3' ) {
			// Reading recieved choice
			h_reads(id_new_socket, &choice, 1);
			//sleep(20);

			// Depending on the choice, different behaviours : 
			switch(choice) {
				case '1' : 
					printf("\n Choix 1: envoyer un fichier client -> serveur.\n");
					// Récupération d'un fichier : on doit écrire dans un fichier le contenu qui va être envoyé.
					// Il faut toujours envoyer des string de taille CONNUE dans les deux sens. 
					// Sinon, il faut faire une lecture octet à octet jusqu'à trouver 'EOF'. Pas très efficace...
					
					// On lit taille du fichier : 
					h_reads(id_new_socket, file_size, INT_SIZE);
					size = (int)strtol(file_size, NULL, 16);
					printf("size of file : %d \n", size);
					
					// On lit le nom du ficher
					h_reads(id_new_socket, file_name, MAX_NAME);
					printf("Name of file : %s \n", file_name);

					// On alloue la mémoire nécessaire.
					// Si il y a une erreur, on ne sort pas
					file_content = (char *) malloc(size);
					if (file_content == NULL) {
						perror("Erreur d'allocation");
						// we do nothing, the error will show in the next function call
					};

					// On lit le contenu fichier : 
					h_reads(id_new_socket, file_content, size);
					//printf("file content : \n%s\n", file_content);

					// On créé nom_fichier et mettons contenu_fichier dedans
					// Si il y a une erreur, on ne sort pas.
					error_code_serv = WriteFile(file_name, size, file_content);
					printf("error code int %d\n", error_code_serv );

					// On libère la mémoire (seulement s'il n'y a pas eu d'erreur avant)
					if (file_content != NULL) {
						free(file_content);
					};

					// Message ok/nok à envoyer au client. We suppose an error only when writing in file.
					// and allocating the memory. On n'envoie seulement que le dernier code d'erreur.
					sprintf(error, "%x", error_code_serv);
					nb_bytes_written = h_writes(id_new_socket, error, INT_SIZE);
					printf("nb bytes written for error %d\n", nb_bytes_written);
					
					// closing connection: only for testing.
					//choice = '3';
					break;
				case '2' : 
					printf("\n Choix 2: envoyer un fichier serveur -> client.\n");
					// Envoyer la liste des fichiers du répertoire courant seulement.
					// The command can be changed here if needed. Note that only files
					// are given here, and should have the proper path if contained in a repertory.
					error_code_serv = GetFileSystem(file_system, MAX_FILESYSTEM_SIZE, "find . -type f | sed 's|^\./||'");
					if (error_code_serv != 0) {
						// send empty string in case of error. Initialising first slot to mark should be enough
						file_system[0] = '\0';
					};
					nb_bytes_written = h_writes(id_new_socket, file_system, MAX_FILESYSTEM_SIZE);
					printf("nb bytes written for file system %d\n", nb_bytes_written);

					// Récuperer le nom du fichier à transferer. 
					h_reads(id_new_socket, file_name, MAX_NAME);
					printf("Name of file : %s \n", file_name);

					// Récuperer la taille du fichier. 
					error_code_serv = GetFileSize(file_name, &size);
					if (error_code_serv != 0) {
						printf("Error nb %d: %s\n", error_code_serv, strerror(error_code_serv)); 
						// Empty string = size of 1.
						size = 1;
					}
					printf("file size : %d", size);

					// Allouer la mémoire necessaire au stockage du fichier.
					file_content = (char *) malloc(size);
					if (file_content == NULL) {
						perror("Erreur d'allocation");
						// we do nothing, the error will show in the next function call
					};
					
					// Récuperer le contenu du fichier
					error_code_serv = ReadFile(file_name, file_content, size);
					if (error_code_serv != 0) {
						printf("Error nb %d: %s\n", error_code_serv, strerror(error_code_serv)); 
						// This holds for the previous error since ReadFile on NULL pointer is sure to give an error.
						// And if we can't read the file size, then the file contents are very probably off limits too.
						// We do not care what is in file_content at the end. 
					};
					//printf("file content : \n%s\n", file_content);

					// Convert size to file_size[]. We need table size(int)+1 for \0.
					sprintf(file_size, "%x", size);

					// Envoyer paramètres nécessaire : taille + content
					// This should be done in one h_writes to have packets as big as possible.
					// Testing in pieces for the moment. And no error management.
					nb_bytes_written = h_writes(id_new_socket, file_size, INT_SIZE);
					printf("nb bytes written for file size %d\n", nb_bytes_written);
					nb_bytes_written = h_writes(id_new_socket, file_content, size);
					printf("nb bytes written for file content %d\n", nb_bytes_written);

					// Liberer la mémoire allouée
					if (file_content != NULL) {
						free(file_content);
					};

					// Message final ok/nok. ON a décidé qu'en cas de pb, le serveur envoie
					// un contenu non pertinant. C'est le message d'erreur qui détermine si ou 
					// ou non le contenu reçu est pertinant. On envoie le dernier code d'erreur
					// étant donné que les erreurs se cascadent : 1->2->3 (et par simplicité surtout)
					sprintf(error, "%x", error_code_serv);
					nb_bytes_written = h_writes(id_new_socket, error, INT_SIZE);
					printf("nb bytes written for error %d\n", nb_bytes_written);

					// closing connection : only for testing.
					//choice = '3';
					break;
				case '3' :
					// The client is closing the connection 
					// Do nothing, we will get out of the while naturally
					printf("\n Choix 3 : le client ferme la connexion.\n");
					break;
				default : 
					// any other number should not exist. As a precaution, close connection.
					printf("\n Choix default %c : ne devrait pas exister. \n", choice);
					printf("On choisi de fermer la connection.\n \n");
					choice = '3';
					break;
			};
		};

		/* 4. FERMETURE DE LA CONNEXION DONNÉE*/
		h_close(id_new_socket); 

	};
	
}

