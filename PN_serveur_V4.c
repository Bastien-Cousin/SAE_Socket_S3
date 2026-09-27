#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <stdbool.h>
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */


#define PORT 5000 //(ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 512

int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue1;
	int socketDialogue2;
	struct sockaddr_in adresseClient1;
	struct sockaddr_in adresseClient2;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageRenvoye[LG_MESSAGE];
	char ip_client1[16];
	int port_ecoute_client1;


	// Crée un socket de communication
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0); 
	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale
	//setsockopt()

	// permettre la réutilisation immédiate du port
	int opt = 1;
	if (setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(-1);
	}

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT); // = 5000 ou plus
	
	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
		perror("bind");
		exit(-2); 
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");
	
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment ! 
	while(1){
		memset(messageRecu, 0, LG_MESSAGE);
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
		
		// c’est un appel bloquant
		// Attente d'un premier joueur
		longueurAdresse = sizeof(adresseClient1);
		socketDialogue1 = accept(socketEcoute, (struct sockaddr *)&adresseClient1, &longueurAdresse);
		if (socketDialogue1 < 0) {
   			perror("accept");
			close(socketDialogue1);
   			close(socketEcoute);
   			exit(-4);
		}

		printf("1er joueur connecté depuis %s:%d\n", 
		       inet_ntoa(adresseClient1.sin_addr), 
		       ntohs(adresseClient1.sin_port));

		// Attente d'un 2ème joueur
		longueurAdresse = sizeof(adresseClient2);
		socketDialogue2 = accept(socketEcoute, (struct sockaddr *)&adresseClient2, &longueurAdresse);
		if (socketDialogue2 < 0) {
   			perror("accept");
			close(socketDialogue1);
			close(socketDialogue2);
   			close(socketEcoute);
   			exit(-4);
		}

		printf("2ème joueur connecté depuis %s:%d\n", 
		       inet_ntoa(adresseClient2.sin_addr), 
		       ntohs(adresseClient2.sin_port));

		// on dit au client 1 qu'il est le joueur 1
		strcpy(messageRenvoye, "PLAYER1\n");
		send(socketDialogue1, messageRenvoye, strlen(messageRenvoye)+1, 0);
		printf("J1 : demande de création du serveur d'écoute\n");
		sleep(1);

		// une fois qu'on recoit les infos du client 1, on les envoie au client 2
		memset(messageRecu, 0, LG_MESSAGE);
		recv(socketDialogue1, messageRecu, LG_MESSAGE, 0);
		sscanf(messageRecu, "%d", &port_ecoute_client1);
		strcpy(ip_client1, inet_ntoa(adresseClient1.sin_addr));
		printf("J1 écoute sur %s:%d\n", ip_client1, port_ecoute_client1);
		
		sprintf(messageRenvoye, "PLAYER2\n%s:%d", ip_client1, port_ecoute_client1);
		send(socketDialogue2, messageRenvoye, strlen(messageRenvoye)+1, 0);
		printf("J2 : envoie des informations de connexion (%s:%d)\n", ip_client1, port_ecoute_client1);

		close(socketDialogue1);
		close(socketDialogue2);

		printf("Joueurs mis en relation. Partie autonome lancée.\n\n");
	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
