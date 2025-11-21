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
#define INT_SIZE 5

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
	int id_socket;
	struct sockaddr_in *p_adr_serv;


	/* 1. CREATION D'UNE SOCKET*/
	id_socket = h_socket(AF_INET, SOCK_STREAM);
	if (id_socket < 0) {
		printf ( "\nERREUR 'h_socket' : Creation socket impossible \n" );
		exit(1);
	};
	

	//* 2. CONNEXION AU SERVEUR ET CONFIGURATION DE LA SOCKET (IP,PORT)*/
	adr_socket(service, serveur, SOCK_STREAM, &p_adr_serv);
	h_connect(id_socket, p_adr_serv);
	// Les deux fonctions renvoient void  par défaut ! pas de tests possibles
	// sans modifications.


	/* 3. ECHANGE DES DONNÉES*/
	// 3.0 Definitions
	char file_system[MAX_FILESYSTEM_SIZE];		// file_system
	char full_file_name[MAX_NAME];				// file name with path
	char file_name[MAX_NAME];					// file name without path
	char *file_content ;						// file content of unknown size at the moment. To be freed !
	char error[INT_SIZE];						// error code in char
	char choice;								// choice of behaviour
	char file_size[INT_SIZE];					// file size in char format
	int nb_bytes_written, nb_bytes_read;		// nb of bytes written and read, only useful for debugging
	int error_code_serv, error_code_clie;		// error codes of server and client repectively. The names are more for information than usefulness
	int size;									// file size in int format

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

				// Récuperer la taille du fichier. Si il y a une erreur, sortir du switch 
				// seulement
				error_code_clie = GetFileSize(full_file_name, &size);
				printf("file size : %d", size);
				if (error_code_clie != 0) {
					printf("Error nb %d: %s\n", error_code_clie, strerror(error_code_clie)); 
					break;
				};
				
				// Allouer la mémoire necessaire au stockage du fichier. Encore, si il
				// y a une erreur, sortir du switch.
				file_content = (char *) malloc(size);
				if (file_content == NULL) {
					perror("Erreur d'allocation");
					break;
				};
				
				// Récuperer le contenu du fichier. Sortir du switch en cas de pb.
				error_code_clie = ReadFile(full_file_name, file_content, size);
				if (error_code_clie != 0) {
					printf("Error nb %d: %s\n", error_code_clie, strerror(error_code_clie)); 
					// And free allocated memory ! (that must have been allocated since we are here)
					free(file_content);
					// before starting back again.
					break;
				};
				//printf("file content : \n%s\n", file_content);

				// Convert size to file_size[]. We need table size(int)+1 for \0.
				sprintf(file_size, "%x", size);

				// Envoyer paramètres nécessaire : choix + taille + nom + content
				// This should be done in one h_writes to have packets as big as possible.
				// Testing in pieces for the moment. And no error management other than
				// eye checking.
				nb_bytes_written = h_writes(id_socket, &choice, 1);
				printf("nb bytes written for choice %d\n", nb_bytes_written);
				nb_bytes_written = h_writes(id_socket, file_size, INT_SIZE);
				printf("nb bytes written for file size %d\n", nb_bytes_written);
				nb_bytes_written = h_writes(id_socket, file_name, MAX_NAME);
				printf("nb bytes written for file name %d\n", nb_bytes_written);
				nb_bytes_written = h_writes(id_socket, file_content, size);
				printf("nb bytes written for file content %d\n", nb_bytes_written);

				// Liberer la mémoire allouée 
				free(file_content);

				// Verifier si il y a eu des erreurs côté serveur. Juste imprimer l'erreur
				// ne pas essayer de la corriger, et ne pas sortir du pgm. Les erreurs sont
				// des codes errno. 
				h_reads(id_socket, error, INT_SIZE);
				error_code_serv = (int)strtol(error, NULL, 16);
				if (error_code_serv == 0) {
					printf("Le transfer de fichier c'est bien passé.\n");
				}
				else {
					printf("Error nb %d: %s\n", error_code_serv, strerror(error_code_serv)); 
				};

				// Sortir pour le prochain choix.
				break;
			case '2' : 
				printf("\n Choix 2: envoyer un fichier serveur -> client.\n");
				// Envoyer notre choix pour que le serveur nous envoie son répertoire courant
				// jusqu'à inode donnée?
				nb_bytes_written = h_writes(id_socket, &choice, 1);
				printf("nb bytes written for choice %d\n", nb_bytes_written);

				// Afficher ce que le serveur nous envoie
				nb_bytes_read = h_reads(id_socket, file_system, MAX_FILESYSTEM_SIZE);
				printf("\nExisting files : \n%s\n", file_system);
				
				// Taper le nom du ficher à récupérer. Ce nom ne peut être que dans la liste
				// des fichiers envoyés. Comme ça, cela nous permet de ne pas envoyer une demande
				// d'existance au serveur. Note : le fichier peut être existant mais pas ouvrable ni
				// modifiable !
				// Error : works with any substring of name ! So not fully bullet-proof
				printf("Veuillez fournir le nom de votre fichier.\n");
				scanf("%s", full_file_name);
				while ( strstr(file_system, full_file_name) == NULL ) {
					printf("Ce fichier n'existe pas. Veuillez entrer un nom correct : ");
					scanf("%s", full_file_name);
				};
				printf("Full file name : %s\n", full_file_name);
				
				// Faire une demande d'envoi de ce fichier vers le serveur : nom du fichier
				nb_bytes_written = h_writes(id_socket, full_file_name, MAX_NAME);
				printf("nb bytes written for file name %d\n", nb_bytes_written);

				// On lit taille du fichier : 
				h_reads(id_socket, file_size, INT_SIZE);
				size = (int)strtol(file_size, NULL, 16);
				printf("size of file : %d \n", size);

				// On alloue la mémoire necessaire
				file_content = (char *) malloc(size);
				if (file_content == NULL) {
					perror("Erreur d'allocation");
					// Il faut fermer la connexion car notre buffer de réception est
					// maintenant plein d'informations que l'on ne peut rien faire avec.
					// Si on ne ferme pas, alors les prochaines lecture seront sur les chaines
					// réceptionnées maintenant !
					// Cependant, on ne peut pas forcer le choix en faisant choice = '3' car alors
					// le serveur ne receverai pas de warning de terminaison de la connexion !
					printf("Veuillez IMPERATIVEMENT fermer la connexion en choisissant l'option (3).\n");
					break;
				};

				// On lit le contenu fichier : 
				h_reads(id_socket, file_content, size);
				//printf("file content : \n%s\n", file_content);

				// On lit le code d'erreur : 
				h_reads(id_socket, error, INT_SIZE);
				error_code_serv = (int)strtol(error, NULL, 16);
				
				// Si il n'y a pas d'erreur coté serveur
				if (error_code_serv == 0) {
					printf("L'envoi du fichier c'est bien passé.\n");
					// On créé nom_fichier et mettons contenu_fichier dedans
					error_code_clie = WriteFile(full_file_name, size, file_content);
					if (error_code_clie != 0) {
						printf("Error nb %d: %s\n", error_code_clie, strerror(error_code_clie));
					};
				}
				else {
					printf("Error nb %d: %s\n", error_code_serv, strerror(error_code_serv)); 
					// Et on n'écrit même pas dans le fichier : on sait que le contenu & taille reçus
					// ne sont pas pertinants.
				};
				
				// On libère la mémoire quoi qu'il se passe.
				free(file_content);

				// Sortir pour le prochain choix.
				break;
			case '3' : 
				printf("\nChoix 3: sortir sans rien faire.\n");
				// Envoyer le choix au serveur. Verifier que le nb de bytes envoyés
				// est correct. Sinon, recommencer. PAs sublime comme test d'erreur.
				//do {
				nb_bytes_written = h_writes(id_socket, &choice, 1);
				printf("nb bytes written for exit %d\n", nb_bytes_written);
				//} while (nb_bytes_written != 2);
				// Ne rien faire côté client, on sort naturellement de la boucle et on ferme la connection
				break;
			default : 
				// au cas où les gens ne savent pas taper, sortir et ne pas leur laisser une autre chance.
				printf("\nUtilisation : (1) envoyer fichier; (2) récupérer fichier; (3) sortir.\n");
				choice = '3';
				// Nb bytes written utile si on veut faire un test que tout c'est bien passé.
				nb_bytes_written = h_writes(id_socket, &choice, 1);
				printf("nb bytes written for exit %d\n", nb_bytes_written);
				break;
		};
	};
	

	/* 4. FERMETURE DE LA SOCKET*/
	h_close(id_socket);
 }
