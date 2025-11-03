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

#include "fon.h"     		/* Primitives de la boite a outils */

// Mon Port serveur
#define SERVICE_DEFAUT "1111"

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
void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
	/* 0. Lexique */ 
	int idSocket;
	int idNewSocket;
	struct sockaddr_in *p_adr_serv;
	struct sockaddr_in *p_adr_client;
	int nb_req_att = 5;
	
	/* 1. Ouverture d'une connection */
	// Création d'une socket
	idSocket = h_socket(AF_INET, SOCK_STREAM);

	// Récupération de l'adresse de la socket, IP, Port
	adr_socket( service, NULL, SOCK_STREAM, &p_adr_serv);
	
	//Paramétrage de la socket 
	h_bind (idSocket, p_adr_serv);

	// Mise en attente de connexion
	h_listen ( idSocket, nb_req_att );

	// Accepter une connexion

	while (1) {
		// 1 : accepter 
		idNewSocket = h_accept(idSocket, p_adr_client);

		// 2 : échanger

		// 3 : Fermer
		h_close(idNewSocket); 

	}
	

/* A completer ... */

}

/******************************************************************************/	

