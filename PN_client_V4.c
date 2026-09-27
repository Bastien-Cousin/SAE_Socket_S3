#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define LG_MESSAGE 512 // taille du message augmentée par rapport aux 256 de base pour supporter les affichages graphiques

// fonction pour mettre à jour l'état du mot (mot avec les _) avec une nouvelle lettre qui a été trouvée
void ajouter_lettre(char lettre[LG_MESSAGE], char mot_a_trouver[], char etat_actuel_mot[]) {
	for (int i = 0; i < strlen(mot_a_trouver); i++) {
		if (mot_a_trouver[i] == lettre[0]) {
			etat_actuel_mot[i] = lettre[0];
		}
	}
}

// fonction pour vérifier tous les cas possibles après que le joueur ai envoyé une lettre et permettre au serveur de savoir quoi renvoyer au client
int verif_cas_lettre(char lettre[256], char mot_a_trouver[], char etat_actuel_mot[], int *etat_pendu, char lettres_choisies[30]){
    if (!((lettre[0] >= 'a' && lettre[0] <= 'z') || (lettre[0] >= 'A' && lettre[0] <= 'Z'))) {
		return 6; // cas 6 : le joueur n'a pas compris le but du jeu et a envoyé autre chose qu'une lettre
	}
	for (int i = 0; i < strlen(lettres_choisies); i++) {
        if (lettres_choisies[i] == lettre[0]) {
            return 1; // cas 1 : le joueur a renvoyé une lettre déjà proposée
        }
    }
    for (int i = 0; i < strlen(mot_a_trouver); i++) {
        if (mot_a_trouver[i] == lettre[0]) {
			ajouter_lettre(lettre, mot_a_trouver, etat_actuel_mot); // on ajoute la lettre dans l'état actuel du mot
			if (strcmp(mot_a_trouver, etat_actuel_mot) == 0) {
				return 4; // cas 4 : le joueur a envoyé la dernière lettre et gagne la partie
			}
			char temp[2] = { lettre[0], '\0' };
			strcat(lettres_choisies, temp); // on ajoute la lettre à la liste des lettres déjà proposées
            return 2; // cas 2 : le joueur a trouvé une lettre présente dans le mot
        }
    }
	*etat_pendu += 1; // le joueur a fait une erreur et une nouvelle partie du pendu se dessine
	if (*etat_pendu >= 6) {
        return 5; // cas 5 : le joueur a fait sa dernière erreur possible et perd la partie
    }
	char temp[2] = { lettre[0], '\0' };
	strcat(lettres_choisies, temp); // on ajoute la lettre à la liste des lettres déjà proposées
    return 3; // cas 3 : le joueur a proposé une lettre qui n'est pas dans le mot à trouver
}

// fonction pour gérer les différents affichages possibles possibles automatiquement en fonction du nombre d'erreurs faites
char* definir_affichage_pendu(int etat_pendu) {

	char *etape_depart =
        "    _______________\n"
        "      |        |\n"
        "      |        |\n"
        "      |\n"
        "      |\n"
        "      |\n"
        "      |\n"
        "      |\n"
        "      |\n"
        "      |\n"
        "  ----------\n";

    char *etape_1 =
        "_______________\n"
        "    |       |\n"
        "    |      _|_\n"
        "    |     |° °|\n"
        "    |     |___|\n"
        "    |\n"
        "    |\n"
        "    |\n"
        "    |\n"
        "    |\n"
        "----------\n";

    char *etape_2 =
        "_______________\n"
        "    |       |\n"
        "    |      _|_\n"
        "    |     |° °|\n"
        "    |     |___|\n"
        "    |       |\n"
        "    |       |\n"
        "    |\n"
        "    |\n"
        "    |\n"
        "----------\n";

    char *etape_3 =
        "_______________\n"
        "    |       |\n"
        "    |      _|_\n"
        "    |     |° °|\n"
        "    |     |___|\n"
        "    |     __|\n"
        "    |       |\n"
        "    |\n"
        "    |\n"
        "    |\n"
        "----------\n";

    char *etape_4 =
        "_______________\n"
        "    |       |\n"
        "    |      _|_\n"
        "    |     |° °|\n"
        "    |     |___|\n"
        "    |     __|__\n"
        "    |       |\n"
        "    |\n"
        "    |\n"
        "    |\n"
        "----------\n";

    char *etape_5 =
        "_______________\n"
        "    |       |\n"
        "    |      _|_\n"
        "    |     |° °|\n"
        "    |     |___|\n"
        "    |     __|__\n"
        "    |       |\n"
        "    |      /\n"
        "    |\n"
        "    |\n"
        "----------\n";

    char *etape_finale =
        "_______________\n"
        "    |       |\n"
        "    |      _|_\n"
        "    |     |x x|\n"
        "    |     |___|\n"
        "    |     __|__\n"
        "    |       |\n"
        "    |      / \\\n"
        "    |\n"
        "    |\n"
        "----------\n";

	switch (etat_pendu) {
		case 0:
			return etape_depart;
		case 1:
			return etape_1;
		case 2:
			return etape_2;
		case 3:
			return etape_3;
		case 4:
			return etape_4;
		case 5:
			return etape_5;
		case 6:
			return etape_finale;
	}
}

// fonction pour gérer le jeu côté joueur 1 ("serveur" / celui qui fait deviner le mot)
void jouer_partie_j1(int descripteurSocket) {
	char messageRecu[LG_MESSAGE];
	char messageRenvoye[LG_MESSAGE];
	int lus;

	char lettres_choisies[30] = ""; // liste des lettres déjà proposées
	int etat_pendu = 0;
	int resultat_verif = 0; // pour stocker le résultat renvoyé par la fonction verif_cas_lettre
	bool partie_finie = false;
	bool mot_valide = false;
	char mot_a_trouver[LG_MESSAGE] = "";
	int longueur_mot;
	char etat_actuel_mot[LG_MESSAGE] = ""; // version caché du mot que l'on va envoyer au client, en affichant que les lettres trouvées

	int compteur = 0;

	// affichage en cas de victoire du joueur qui cherche le mot
	char *affichage_victoire = 
		"_______________\n"
    	"    |        |\n"
    	"    |  `     |     `          `\n"
    	"    |`        `          `\n"
    	"    |      `       `\n"
    	"    |           ___         `     `\n"
    	"    |   `      |° °|    `\n"
    	"    |          |_O_|          `\n"
    	"    |         \\__|__/ `\n"
    	"    |    `       |       \n"
		"----------     _/ \\_\n";

	printf("Commencez par choisir le mot à faire deviner à l'autre joueur : \n");
	scanf("%s", mot_a_trouver);

	// vérifier la validité du mot : pas d'autres caractères que des lettres minuscules ou majuscules
	while (mot_valide != true) {
		compteur = 0;
		for (int i = 0; i < strlen(mot_a_trouver); i++) {
			if (((mot_a_trouver[i] >= 'a' && mot_a_trouver[i] <= 'z') || (mot_a_trouver[i] >= 'A' && mot_a_trouver[i] <= 'Z'))) {
				compteur += 1;
			}
		}
		if (compteur == strlen(mot_a_trouver)) {
			mot_valide = true;
		} else {
			printf("Le mot n'est pas valide ! Remettez-en un : \n");
			scanf("%s", mot_a_trouver);
		}
	}
	longueur_mot = strlen(mot_a_trouver);
	// générer etat_actuel_mot automatiquement en fonction de la longueur du mot à trouver
	for (int i = 0; i < longueur_mot; i++) {
		etat_actuel_mot[i] = '_';
		mot_a_trouver[i] = (char) toupper((unsigned char) mot_a_trouver[i]);
	}
	etat_actuel_mot[longueur_mot] = '\0';

	sleep(1); // Délai pour éviter que le client soit plus rapide que l'autre client
	sprintf(messageRenvoye, "Début du jeu : mot de %d lettres\n\n%s\n\n", longueur_mot, definir_affichage_pendu(etat_pendu));
	send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);

	while (partie_finie != true) {
		sleep(1);
		send(descripteurSocket, etat_actuel_mot, strlen(etat_actuel_mot)+1, 0);
		printf("Le joueur 2 choisit une lettre ou un mot...\n\n");

		// On réception les données du client (cf. protocole)
		//lus = read(socketDialogue1, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
		memset(messageRecu, 0, LG_MESSAGE);
		lus = recv(descripteurSocket, messageRecu, LG_MESSAGE*sizeof(char),0); // ici appel bloquant
		switch(lus) {
			case -1 : /* une erreur ! */ 
				perror("read"); 
				close(descripteurSocket); 
				exit(-5);
			case 0  : /* la socket est fermée */
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
				close(descripteurSocket);
				return;
			default:  /* réception de n octets */
				// si le message envoyé est une lettre
				if (lus == 2) {
					printf("Lettre reçue : %s (%d octets)\n", messageRecu, lus);
					// vérifier les différents cas que le jeu doit prévoir quand une lettre est donnée
					resultat_verif = verif_cas_lettre(messageRecu, mot_a_trouver, etat_actuel_mot, &etat_pendu, lettres_choisies);
					switch(resultat_verif) {
						case 0: // cas 0 : une erreur imprévue
							fprintf(stderr, "Erreur avec le résultat de la fonction verif_cas_lettre !\n\n");
							close(descripteurSocket);
							return;
						case 1: // cas 1 : le joueur a renvoyé une lettre déjà proposée
							strcpy(messageRenvoye, "Vous avez redonné une lettre déjà donnée. Choisissez en une autre.\n\n");
							send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
							break;
						case 2: // cas 2 : le joueur a trouvé une lettre présente dans le mot
							strcpy(messageRenvoye, "Vous avez trouvé une des lettres du mot !\n\n");
							send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
							sleep(1);
							printf("L'autre joueur a trouvé une des lettres du mot !\n\n");
							break;
						case 3: // cas 3 : le joueur a proposé une lettre qui n'est pas dans le mot à trouver
							sprintf(messageRenvoye, "Cette lettre n'est pas dans le mot ! Il vous reste %d tentatives avant d'être pendu.\n\n%s\n\n", 6 - etat_pendu, definir_affichage_pendu(etat_pendu));
							send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
							sleep(1);
							printf("L'autre joueur s'est trompé de lettre ! Il lui reste %d tentatives avant d'être pendu.\n\n%s\n\n", 6 - etat_pendu, definir_affichage_pendu(etat_pendu));
							break;
						case 4: // cas 4 : le joueur a envoyé la dernière lettre et gagne la partie
							sprintf(messageRenvoye, "Vous avez trouvé le mot ! Le mot était bien : %s\n\n%s\n", mot_a_trouver, affichage_victoire);
							send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
							sleep(1);
							printf("L'autre joueur à trouvé le mot, vous avez perdu ! Le mot était : %s\n\n%s\n", mot_a_trouver, affichage_victoire);
							partie_finie = true;
							break;
						case 5: // cas 5 : le joueur a fait sa dernière erreur possible et perd la partie
							sprintf(messageRenvoye, "Vous n'avez pas trouvé le mot et vous êtes pendus ! Le mot était : %s\n\n%s\n", mot_a_trouver, definir_affichage_pendu(6));
							send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
							sleep(1);
							printf("Votre adversaire s'est trompé et à été pendu, vous avez donc gagné ! Le mot était : %s\n\n%s\n", mot_a_trouver, definir_affichage_pendu(6));
							partie_finie = true;
							break;
						case 6: // cas 6 : le joueur n'a pas compris le but du jeu et a envoyé autre chose qu'une lettre
							strcpy(messageRenvoye, "Erreur, ce que vous avez rentré n'est pas une lettre ! Choisissez une lettre.\n\n");
							send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
							break;
					}
				// si le message envoyé est un mot
				} else {
					if (strcmp(messageRecu, mot_a_trouver) == 0) { // cas où le mot est trouvé
						sprintf(messageRenvoye, "Vous avez gagné ! Le mot était bien %s\n\n%s\n", mot_a_trouver, affichage_victoire);
						send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
						sleep(1);
						printf("L'autre joueur à deviner le bon mot, vous avez perdu ! Le mot était : %s\n\n%s\n", mot_a_trouver, affichage_victoire);
						partie_finie = true;
					} else { // cas où le mot n'est pas trouvé
						sprintf(messageRenvoye, "Vous vous êtes trompé de mot et vous êtes pendus ! Le mot était : %s\n\n%s\n", mot_a_trouver, definir_affichage_pendu(6));
						send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1, 0);
						sleep(1);
						printf("Votre adversaire a deviné le mauvais mot et à été pendu, vous avez donc gagné ! Le mot était : %s\n\n%s\n", mot_a_trouver, definir_affichage_pendu(6));
						partie_finie = true;
					}
				}
			
		}
	}
}

// fonction pour gérer le jeu côté joueur 2 (client / celui qui doit deviner le mot)
void jouer_partie_j2(int descripteurSocket) {
	char messageRecu[LG_MESSAGE];
	char messageRenvoye[LG_MESSAGE];
	int nb; /* nb d’octets écrits et lus */

	bool partie_finie = false;
	int choix_action = 0;

	printf("Le joueur 1 choisit le mot à deviner...\n");

	recv(descripteurSocket, messageRecu, 256, 0);
	printf("Mot choisi !\n\n");
	printf("%s\n", messageRecu);
	while (partie_finie != true) {
		memset(messageRecu, 0, LG_MESSAGE);
		recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
		// petit menu pour choisir si l'on veut envoyer une lettre ou le mot
		printf("Mot : %s\n", messageRecu);
		printf("Que voulez-vous faire ?\n");
		printf("- Choisir une lettre (1)\n");
		printf("- Deviner le mot (2)\n");
		scanf("%d", &choix_action);
		// on traite le choix du joueur
		if (choix_action == 1) {
			printf("Choisissez une lettre : \n");
			scanf("%1s", messageRenvoye);
		} else if (choix_action == 2) {
			printf("Devinez le mot (attention : 1 seul essai) : \n");
			scanf("%s", messageRenvoye);
		}
		// on gère la casse en passant le message de l'utilisateur en majuscules
		for (int i = 0; messageRenvoye[i] != '\0'; i++) {
			messageRenvoye[i] = (char) toupper((unsigned char) messageRenvoye[i]);
		}

		// Envoi du message
		//switch(nb = write(descripteurSocket, buffer, strlen(buffer))){
		switch(nb = send(descripteurSocket, messageRenvoye, strlen(messageRenvoye)+1,0)){
			case -1 : /* une erreur ! */
				perror("Erreur en écriture...");
				close(descripteurSocket);
				exit(-3);
			case 0 : /* le socket est fermée */
				fprintf(stderr, "Le socket a été fermée par le serveur !\n\n");
				return;
			default: /* envoi de n octets */
				printf("Message %s envoyé! (%d octets)\n\n", messageRenvoye, nb);
				memset(messageRecu, 0, LG_MESSAGE);
				recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
				// on traite la réponse de l'autre client
				printf("Réponse de l'autre joueur : %s", messageRecu);
				// si la réponse contient la "clé" présente dans tous les messages de fin de partie, la partie est terminée
				if (strstr(messageRecu, "Le mot était") != NULL) {
					partie_finie = true;
					break;
				}
		}
	}
}

// programme principal
int main(int argc, char *argv[]){
	int descripteurSocketServeur;
	int descripteurSocketPeer;
	int socketEcoute;
	struct sockaddr_in sockaddrServeur;
	struct sockaddr_in pointDeRencontreLocal;
	struct sockaddr_in adressePeer;
	socklen_t longueurAdresse;

	char buffer[256]; // buffer stockant le message
	char messageEnvoye[256];
	int nb; /* nb d’octets écrits et lus */

	char ip_serveur[16];
	int port_serveur;
	char peer_ip[16];
	int peer_port;
	int mon_port_ecoute;

	// Pour pouvoir contacter le serveur, le client doit connaître son adresse IP et le port de comunication
	// Ces 2 informations sont passées sur la ligne de commande
	// Si le serveur et le client tournent sur la même machine alors l'IP locale fonctionne : 127.0.0.1
	// Le port d'écoute du serveur est 5000 dans cet exemple, donc en local utiliser la commande :
	// ./client_base_tcp 127.0.0.1 5000
	if (argc>1) { // si il y a au moins 2 arguments passés en ligne de commande, récupération ip et port
		strncpy(ip_serveur,argv[1],16);
		sscanf(argv[2],"%d",&port_serveur);
	}else{
		printf("USAGE : %s ip port\n",argv[0]);
		exit(-1);
	}

	// Crée un socket de communication
	descripteurSocketServeur = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if(descripteurSocketServeur < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n\n", descripteurSocketServeur);


	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant la machine distante)
	// Obtient la longueur en octets de la structure sockaddr_in
	longueurAdresse = sizeof(sockaddrServeur);
	// Initialise à 0 la structure sockaddr_in
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &sockaddrDistant
	memset(&sockaddrServeur, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	sockaddrServeur.sin_family = AF_INET;
	// On choisit le numéro de port d’écoute du serveur
	sockaddrServeur.sin_port = htons(port_serveur);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_serveur, &sockaddrServeur.sin_addr);

	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocketServeur, (struct sockaddr *)&sockaddrServeur,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocketServeur);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n",ip_serveur,port_serveur);
	printf("Joueurs trouvés : 1/2. En attente d'un 2ème joueur avant création de la partie...\n\n");

	memset(buffer, 0, 256);
	recv(descripteurSocketServeur, buffer, 256, 0);
	// si le serveur dit que ce client est le joueur 1
	if (strcmp(buffer, "PLAYER1\n") == 0) {

		// on se sert du client 1 comme serveur pour accueillir le client 2
		socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
		if (socketEcoute < 0) {
			perror("socket écoute");
			exit(-3);
		}

		longueurAdresse = sizeof(pointDeRencontreLocal);
		memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
		pointDeRencontreLocal.sin_family = PF_INET;
		pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
		pointDeRencontreLocal.sin_port = 0; // port aléatoire choisi par le système

		if (bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse) < 0) {
			perror("bind");
			exit(-4);
		}
		
		// récupérer le port attribué par le système
		getsockname(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, &longueurAdresse);
		mon_port_ecoute = ntohs(pointDeRencontreLocal.sin_port);
		printf("Serveur d'écoute créé sur le port %d\n", mon_port_ecoute);
		
		if (listen(socketEcoute, 1) < 0) {
			perror("listen");
			exit(-5);
		}

		sprintf(messageEnvoye, "%d", mon_port_ecoute);
		send(descripteurSocketServeur, messageEnvoye, strlen(messageEnvoye)+1, 0);
		printf("Port d'écoute communiqué au serveur\n");
		
		// fermer la connexion au serveur
		close(descripteurSocketServeur);

		// maintenant que le serveur est créer, on attend la connexion du client 2
		printf("En attente du joueur 2...\n");
		longueurAdresse = sizeof(adressePeer);
		descripteurSocketPeer = accept(socketEcoute, (struct sockaddr *)&adressePeer, &longueurAdresse);
		if (descripteurSocketPeer < 0) {
			perror("accept");
			exit(-6);
		}
		printf("Joueur 2 connecté depuis %s:%d\n\n", 
		       inet_ntoa(adressePeer.sin_addr), 
		       ntohs(adressePeer.sin_port));
		close(socketEcoute);

		// quand tout est prêt, on lance la partie
		jouer_partie_j1(descripteurSocketPeer);
	} else {
		// parser "PLAYER2\nIP:PORT"
		char *line1 = strtok(buffer, "\n");
		char *line2 = strtok(NULL, "\n");
		
		if (line1 && strcmp(line1, "PLAYER2") == 0 && line2) {
			printf("Joueurs trouvés : 2/2. Lancement...\n\n");
			
			// parser IP:PORT
			sscanf(line2, "%[^:]:%d", peer_ip, &peer_port);
			printf("Infos du joueur 1 : %s:%d\n", peer_ip, peer_port);
			
			// fermer la connexion au serveur
			close(descripteurSocketServeur);
			
			sleep(1);
			
			descripteurSocketPeer = socket(AF_INET, SOCK_STREAM, 0);
			if (descripteurSocketPeer < 0) {
				perror("socket peer");
				exit(-3);
			}
			
			memset(&adressePeer, 0x00, sizeof(adressePeer));
			adressePeer.sin_family = AF_INET;
			adressePeer.sin_port = htons(peer_port);
			inet_aton(peer_ip, &adressePeer.sin_addr);
			
			printf("Connexion au joueur 1 (%s:%d)...\n", peer_ip, peer_port);
			if (connect(descripteurSocketPeer, (struct sockaddr *)&adressePeer, sizeof(adressePeer)) == -1) {
				perror("Erreur de connexion au joueur 1");
				close(descripteurSocketPeer);
				exit(-7);
			}
			printf("Connecté au joueur 1 !\n\n");
			
			// quand le client 2 a réussi à se connecter au client 1, on lance la partie de son côté aussi
			jouer_partie_j2(descripteurSocketPeer);
		}
	}

	// On ferme la ressource avant de quitter
	close(descripteurSocketPeer);

	return 0;
}
