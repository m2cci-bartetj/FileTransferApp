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
	int idSocket;
	int idNewSocket;
	struct sockaddr_in *p_adr_serv;
	struct sockaddr_in p_adr_client;
	int nb_req_att = 5;

	char choice ;							// choice of the procedure to follow
	char file_size[INT_SIZE];				// file size in char format
	int size;								// file size in int format
	char file_name[MAX_NAME];				// Name of the file
	char * file_content;					// pointer for the content of the file. Allocated dynamically => must be freed
	int nb_bytes_written, nb_bytes_read;	// Mostly used for visual error handling
	int error_code_serv, error_code_clie;	// Error handling
	char file_system[MAX_FILESYSTEM_SIZE];	// file_system

	/* 1. CONFIGURATION D'UNE CONNEXION*/
	// 1.1. Création d'une socket
	idSocket = h_socket(AF_INET, SOCK_STREAM);
	// /!\ TEST A FAIRE

	// 1.2. Paramétrage de la socket. Récupération de l'adresse de la socket, IP, Port
	adr_socket(service, NULL, SOCK_STREAM, &p_adr_serv);
	h_bind(idSocket, p_adr_serv);
	// /!\ TEST A FAIRE

	// 1.3. Mise en écoute de connexion
	h_listen( idSocket, nb_req_att );
	// /!\ TEST A FAIRE

	// 1.4 boucle infinie d'accpetation et de fermeture de connexion
	while (1) {
		/* 2. ACCEPTATION D'UNE CONNEXION*/
		idNewSocket = h_accept(idSocket, &p_adr_client);
		// /!\ TEST A FAIRE

		/* 3. ECHANGE DE DONNÉES*/
		/* while socket status = Established, continuer. Sinon, cela veut dire que le client a 
		envoyé un close => il faut donc s'arrêter. Lorsque le client s'arrête, il envoie char '3'.
		A noter que cet échange n'est posible que si tout c'est bien passé au-dessu (ce qui n'est pas
		testé pour le moment)*/
		
		choice = '0';
		while ( choice != '3' ) {
			// Reading recieved choice
			h_reads(idNewSocket, &choice, 1);
			//sleep(20);

			// Depending on the choice, different behaviours : 
			switch(choice) {
				case '1' : 
					printf("\n Choix 1: envoyer un fichier client -> serveur.\n");
					// Récupération d'un fichier : on doit écrire dans un fichier le contenu qui va être envoyé.
					// Il faut toujours envoyer des string de taille CONNUE dans les deux sens. 
					// Sinon, il faut faire une lecture bit à bit jusqu'à trouver '\0'. Pas très efficace... Meilleur manière ?
					
					// On lit taille du fichier : 
					h_reads(idNewSocket, file_size, INT_SIZE);
					size = (int)strtol(file_size, NULL, 16);
					printf("size of file : %d \n", size);
					
					// On lit le nom du ficher
					h_reads(idNewSocket, file_name, MAX_NAME);
					printf("Name of file : %s \n", file_name);

					// On lit le contenu fichier : 
					file_content = malloc(size);
					h_reads(idNewSocket, file_content, size);
					printf("file content : \n%s\n", file_content);

					// On créé nom_fichier et mettons contenu_fichier dedans
					// Si il y a une erreur, on ne sort pas.
					error_code_serv = WriteFile(file_name, size, file_content);
					free(file_content);

					// Message ok/nok à envoyer au client ? We suppose error in only in writing in file.
					//nb_bytes_written = h_writes(idNewSocket, (char *) &error_code_serv, INT_SIZE);
					//printf("nb bytes written for error %d\n", nb_bytes_written);
					
					// closing connection: only for testing.
					//choice = '3';
					break;
				case '2' : 
					printf("\n Choix 2: envoyer un fichier serveur -> client.\n");
					// Envoyer la liste des fichiers du répertoire courant seulement.
					// The command can be changed here if needed. Note that only files
					// are given here, and should have the proper path if contained in a repertory.
					GetFileSystem(file_system, MAX_FILESYSTEM_SIZE, "find . -type f | sed 's/^.\///'");
					nb_bytes_written = h_writes(idNewSocket, file_system, MAX_FILESYSTEM_SIZE);
					printf("nb bytes written for file system %d\n", nb_bytes_written);

					// Récuperer le nom du fichier à transferer. 
					h_reads(idNewSocket, file_name, MAX_NAME);
					printf("Name of file : %s \n", file_name);

					// Récuperer la taille du fichier & son contenu
					error_code_clie = ReadFile(file_name, &file_content, &size);
					//CheckError(error_code_clie, 1);
					printf("file content : \n%s\n", file_content);
					printf("file size : %d\n", size);

					// Convert size to file_size[]. We need table size(int)+1 for \0.
					sprintf(file_size, "%x", size);

					// Envoyer paramètres nécessaire : taille + content
					// This should be done in one h_writes to have packets as big as possible.
					// Testing in pieces for the moment. And no error management.
					nb_bytes_written = h_writes(idNewSocket, file_size, INT_SIZE);
					printf("nb bytes written for file size %d\n", nb_bytes_written);
					nb_bytes_written = h_writes(idNewSocket, file_content, size);
					printf("nb bytes written for file content %d\n", nb_bytes_written);

					// Liberer la mémoire allouée 
					free(file_content);

					// Message final ok/nok

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
		h_close(idNewSocket); 

	};
	
}

