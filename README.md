# SAE Socket - Décembre 2025

## Développement de la V0 à la V4 dans le cadre de la SAE Socket du 8 décembre au 17 décembre 2025

## Equipe composée de :
- COUSIN Bastien TPB
- DELDALLE Pierre TPB
- DEQUIDT Clément TPB
- GROUÉ Sébastien TPD

## Description du projet : 
Ce projet consiste à créer plusieurs versions du jeu du pendu en C. En commençant par une version (V0) ou un joueur joue seul et essaye de trouver un seul mot et arriver à une version (V4) complète où plusieurs joueurs peuvent se connecter et jouer ensemble au pendu.

## Fonctionnalités : 
# V0 :
- Un joueur se connecte au serveur pour deviner le mot.
- Un serveur fait deviner le mot, qui est toujours le même.

# V1 :
- Deux joueurs peuvent se connecter au serveur pour jouer l'un contre l'autre.
- Un serveur fait la passerelle entre les deux joueurs pour faire deviner le même mot.

# V2 : 
- Deux joueurs jouent ensemble, l'un donne et fait deviner le mot, l'autre le cherche.
- Un serveur fait la passerelle entre les deux mais ne s'occupe que de la communication, il n'est plus dans la partie.

# V3 : 
- Cette version est comme la V2 sauf que le serveur reste allumé tout le temps pour attendre de nouveaux joueurs.

# V4 : 
- Cette version se base encore une fois sur la V2 mais cette fois si, le serveur ne sert plus qu'à mettre en communication des joueurs.

## Utilisation de la version finale (V4) : 
Pour pouvoir faire fonctionner le jeu, il vous faut 2 ordinateurs de préférence (un seul suffit mais c'est mieux à deux.). Il faut dans un premier temps, être sur le même réseau et sur les deux pc, compiler les deux fichiers : gcc PN_serveur_V4.c -o serveur_V4 puis gcc PN_client_V4.c -o client_V4 . Sur le premier ordinateur, dans un terminal faire ./serveur_V4
puis dans un deuxième terminal faire hostname -i pour récupérer l'ip de son ordinateur puis faire ./client_V4 10.2.3.170 5000 (ici on prend comme ip 10.2.3.170 et le port est 5000).
Sur le deuxième ordinateur après avoir compilé comme sur le premier ordinateur on se connecte directement en tant que deuxième client en faisant ./client_V4 10.2.3.170 5000 

## Notes : 
- Les IA génératives ont été utilisés uniquement pour comprendre et nous aider à corriger les bugs que nous rencontrions dans le terminal et que nous n'arrivions pas à régler seuls, aucun code n'a été généré avec leurs aides.
