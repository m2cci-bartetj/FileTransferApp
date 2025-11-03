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

#include "fon.h"   		/* primitives de la boite a outils */

// Port et IP par défaut pour le serveur a contacter
#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

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
	//Lexique 
	int idSocket;
	struct sockaddr_in *p_adr_serv;

	// Création d'une socket
	idSocket = h_socket(AF_INET, SOCK_STREAM);
	
	// Connexion au serveur et association IP, Port à la socket
	adr_socket( service, serveur, SOCK_STREAM, &p_adr_serv);

	h_connect( idSocket, p_adr_serv );

	/* Envoie de fichier
		FILE* fichier = fopen(const char *path = chemin d'acces du fichier,
				 char* mode_ouverture = r,
				 )

		int fclose(FILE* fichier)

		int read(int fd = fichier,
				 void *buf,
				 size_t nbytes)
	 */

	 FILE* f;
	 char nomDuFichier[200] ="./fichiers/fichier.txt";
	 const int BUFFER_SIZE = 2000;
	 char buffer[BUFFER_SIZE];
	
	 f = fopen(nomDuFichier, "r");
	 if(f==NULL) {
		perror("Erreur d'ouverture en lecture\n");
	 } 

	 else {
	 	char size = 0;
	 	while ( (size < BUFFER_SIZE) && (fread((buffer+size), 1, 1, f) !=0) ) {
			size++;
		};

		h_writes(idSocket, &size, sizeof(size));
		//Envoi du fichier contenu dans le buffer
		h_writes(idSocket, buffer, size);
	 
		if(fclose(f) != 0) {
			perror("Erreur lors de la fermeture du fichier\n");
	 	}
	};
	

/* a completer .....  */

 }

/*****************************************************************************/

