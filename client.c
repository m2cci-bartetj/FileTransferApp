/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/	


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <errno.h>
#include <unistd.h>

#include "fon.h"   		/* primitives de la boite a outils */
#include "file.h"	    /* primitives pour la manipulation de fichiers*/

// Port et IP par défaut pour le serveur a contacter
#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define MAX_FILESYSTEM_SIZE 500
#define MAX_NAME 100		// since FILEMAX_NAME = 4096 appears to be too big. Creates a h_accept :acceptation impossible.
#define INT_SIZE 4

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{
	/* 0. DEFINITIONS */
	int idSocket;
	struct sockaddr_in *p_adr_serv;


	/* 1. CREATION D'UNE SOCKET*/
	idSocket = h_socket(AF_INET, SOCK_STREAM);
	// /!\ TEST A FAIRE
	

	//* 2. CONNEXION AU SERVEUR ET CONFIGURATION DE LA SOCKET (IP,PORT)*/
	adr_socket(service, serveur, SOCK_STREAM, &p_adr_serv);
	h_connect(idSocket, p_adr_serv);
	// /!\ TEST A FAIRE


	/* 3. ECHANGE DES DONNÉES*/
	// 3.0 Definitions
	char full_file_name[MAX_NAME];			// file name with path
	char file_name[MAX_NAME];				// file name without path
	char file_system[MAX_FILESYSTEM_SIZE];		// file_system
	char * file_content ;						// Of unknown size at the moment
	int file_size;
	int nb_bytes_written, nb_bytes_read;
	char error_code;
	char choice;
	char size[INT_SIZE];						// Stocker file_size in char

	// 3.1 Choix de la procédure : envoyer ou récupérer un fichier ou sortir ?
	choice = '0';
	while (choice != '3') {
		printf("\n Que voulez-vous faire : \n");
		printf(" 1. Envoyer un fichier ? \n");
		printf(" 2. Récupérer un fichier ? \n");
		printf(" 3. Sortir sans rien faire ? \n");
		printf("Tapez 1, 2 ou 3 : ");
		//fflush(stdin);
		scanf(" %c", &choice);
		//fflush(stdin);

		switch(choice)
		{
			case '1' :
				printf("\n Choix 1: envoyer un fichier client -> serveur.\n");
				// A noter que l'on ne dit pas où stocker ce fichier dans le serveur.
				// Cela pourrait être une option supplémentaire. Par défault, ce sera
				// donc dans le même répertoire que là où l'on lance ./serveur
							
				// Taper le nom du ficher à envoyer
				printf("\n Veuillez fournir le nom de votre fichier : ");
				scanf("%s", full_file_name);
				
				// Vérifier que ce fichier existe en interne
				// Ce test peut être fait lors de la lecture du fichier.
				// Note : ce fichier peut exister, mais dépasser la limite max de taille du nom de fichier
				// Dans ce cas, tant pis pour l'utilisateur.
				while ( access(full_file_name, F_OK) == -1 ) {
					printf("Ce fichier n'existe pas. Veuillez entrer un nom correct : ");
					scanf("%s", full_file_name);
				};
				printf("Full file name : %s\n", full_file_name);
				
				// Récupérer seulement le nom du fichier hors path
				GetFileName(full_file_name, file_name);

				// Récuperer la taille du fichier & son contenu
				ReadFile(full_file_name, &file_content, &file_size);

				// Envoyer paramètres nécessaire : choix + taille + nom + content
				// This should be done in one h_writes to have packets as big as possible.
				// Testing in pieces for the moment.
				h_writes(idSocket, &choice, 1);
				h_writes(idSocket, (char *) &file_size, INT_SIZE);
				h_writes(idSocket, file_name, FILENAME_MAX);
				h_writes(idSocket, file_content, file_size);

				// Liberer la mémoire allouée 
				free(file_content);
				sleep(10);
				
				/*// Verifier quelles erreurs, si erreurs ont été provoquées côté serveur
				// Juste imprimer l'erreur. Note : on ne peut pas en avoir plus que 255.
				h_reads(idSocket, &error_code, 1);
				switch(error_code) {
					case '1' :
						printf("Error in ... \n");
						break;
					case '2' :
						printf("Error in ... \n");
						break;
					case '3' :
						printf("Error in ... \n");
						break;
					case '4' :
						printf("Error in ... \n");
						break;
					case '5' :
						printf("Error in ... \n");
						break;
					case '6' :
						printf("Error in ... \n");
						break;
					case '7' :
						printf("Error in ... \n");
						break;
					default :
						printf("Not yet registered error.\n");
						break;
				};*/
				break;
			/*case '2' : 
				printf("Choix 2: récupérer un fichier.\n");
				// Envoyer notre choix pour que le serveur nous envoie son répertoire courant
				// jusqu'à inode donnée?
				nb_bytes_written = h_writes(idSocket, &choice, 1);

				// Afficher ce que le serveur nous envoie
				nb_bytes_read = h_reads(idSocket, file_system, MAX_FILESYSTEM_SIZE);
				
				// Taper le nom du ficher à récupérer
				printf("Veuillez fournir le nom de votre fichier.\n");
				fgets(full_file_name, FILENAME_MAX, stdin);
				
				// Faire une demande d'envoi de ce fichier vers le serveur : nom du fichier
				nb_bytes_written = h_writes(idSocket, &full_file_name, FILENAME_MAX);

				// Récupérer le code d'erreur
				nb_bytes_read = h_reads(idSocket, &error_code, 1);

				// Si = 0 : le fichier n'est pas connu. Si = 1, il l'est.
				// Il peut avoir d'autres codes d'erreur ! => faire un switch.
				if (error_code) {
					printf("Le fichier n'est pas connu.\n");
				}
				else {
					// Récuperer la taille du fichier
					// We suppose sizeof(int) in the same on both computers.
					nb_bytes_read = h_reads(idSocket, &size, sizeof(int));

					// Convertir cette taille en int, et allouer le bon espace
					ConvertChar2Int(&file_size, size);
					char * file_content = malloc(file_size);

					// Lire le contenu du fichier récupéré.
					nb_bytes_read = h_reads(idSocket, file_content, file_size);

					// Ecrire dans un nouveau fichier 
					WriteFile(file_name, file_size, file_content);

					// Liberer
					free(file_content);
					
				};
				break;*/
			case '3' : 
				printf("Choix 3: sortir sans rien faire.\n\n");
				// Envoyer le choix au serveur. Verifier que le nb de bytes envoyés
				// est correct. Sinon, recommencer. PAs sublime comme test d'erreur.
				//do {
				nb_bytes_written = h_writes(idSocket, &choice, 1);
				//} while (nb_bytes_written != 2);
				// Ne rien faire côté client, on sort naturellement de la boucle et on ferme la connection
				break;
			default : 
				// au cas où les gens ne savent pas taper, sortir et ne pas leur laisser une autre chance.
				printf("\n Utilisation : (1) envoyer fichier; (2) récupérer fichier; (3) sortir. \n\n");
				choice = '3';
				// Nb bytes written utile si on veut faire un test que tout c'est bien passé.
				nb_bytes_written = h_writes(idSocket, &choice, 1);
				break;
		};
	};
	

	/* 4. FERMETURE DE LA SOCKET*/
	h_close(idSocket);
 }
