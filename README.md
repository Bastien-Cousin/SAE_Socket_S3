# 🔌 Jeu du pendu multijoueur en C avec sockets

Projet réseau développé en **C** autour du jeu du pendu, avec plusieurs versions successives allant d'une architecture simple à une version multijoueur reposant sur une communication client/serveur par sockets.

L'objectif principal du projet était de comprendre progressivement les mécanismes de communication réseau, la gestion de plusieurs clients et l'évolution du rôle d'un serveur au sein d'une application distribuée.

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=000000)

---

## 🎮 À propos du projet

Le projet consiste à développer plusieurs versions du jeu du pendu en faisant évoluer progressivement son architecture réseau.

Chaque version ajoute de nouvelles contraintes afin de passer d'un fonctionnement client/serveur simple à une version dans laquelle plusieurs joueurs peuvent se connecter et jouer ensemble.

Cette progression permet d'aborder étape par étape différentes notions de programmation réseau :

- communication entre un client et un serveur ;
- échange de données via le réseau ;
- gestion de plusieurs joueurs ;
- maintien d'un serveur disponible ;
- séparation des responsabilités entre serveur et clients ;
- mise en relation de plusieurs machines.

---

## 🚀 Évolution du projet

### V0 — Jeu simple client / serveur

- un joueur se connecte au serveur ;
- le serveur possède un mot fixe à faire deviner ;
- le client envoie ses propositions au serveur.

Cette première version permet de mettre en place les bases de la communication réseau entre deux programmes.

### V1 — Deux joueurs sur un même serveur

- deux joueurs peuvent se connecter au serveur ;
- les deux clients tentent de deviner le même mot ;
- le serveur gère la partie et sert d'intermédiaire entre les joueurs.

Cette version introduit la gestion de plusieurs clients connectés au même serveur.

### V2 — Partie entre deux joueurs

- deux joueurs jouent ensemble ;
- un joueur choisit le mot à faire deviner ;
- le second joueur tente de le trouver ;
- le serveur sert principalement d'intermédiaire pour leurs communications.

La logique du jeu est ainsi progressivement déplacée du serveur vers les clients.

### V3 — Serveur persistant

Cette version reprend le fonctionnement de la V2 tout en faisant évoluer le comportement du serveur.

Le serveur reste actif afin de pouvoir attendre de nouvelles connexions et permettre à de nouveaux joueurs de se connecter sans devoir relancer manuellement le programme.

### V4 — Mise en relation des joueurs

La dernière version pousse encore plus loin la séparation des responsabilités :

- le serveur reste disponible pour recevoir de nouvelles connexions ;
- il permet aux joueurs de rejoindre le système ;
- il sert principalement à mettre les joueurs en communication ;
- la logique de la partie est gérée côté clients.

Cette version constitue l'aboutissement de l'évolution de l'architecture réseau réalisée au cours du projet.

---

## 🌐 Communication réseau

Le projet repose sur l'utilisation de **sockets réseau en C**.

Le principe général consiste à lancer un serveur sur une machine accessible depuis le réseau, puis à connecter plusieurs clients grâce à son adresse IP et à un port donné.

Au fil des différentes versions, le rôle du serveur évolue progressivement :

1. gestion d'une partie simple ;
2. coordination de plusieurs joueurs ;
3. intermédiaire de communication entre les clients ;
4. serveur persistant permettant la mise en relation des joueurs.

Cette évolution permet d'expérimenter plusieurs organisations possibles d'une architecture client/serveur.

---

## ▶️ Utilisation de la version finale

La version finale peut fonctionner sur une seule machine, mais l'utilisation de **deux ordinateurs connectés au même réseau** permet de reproduire plus fidèlement son fonctionnement.

### Compilation

Compiler le serveur :

```bash
gcc PN_serveur_V4.c -o serveur_V4
```

Compiler le client :

```bash
gcc PN_client_V4.c -o client_V4
```

### Lancement du serveur

```bash
./serveur_V4
```

Il est ensuite possible de récupérer l'adresse IP de la machine hébergeant le serveur :

```bash
hostname -i
```

### Connexion d'un client

```bash
./client_V4 <adresse_ip_du_serveur> <port>
```

Exemple :

```bash
./client_V4 10.2.3.170 5000
```

Un second joueur peut ensuite lancer le même client depuis une autre machine en utilisant la même adresse IP et le même port.

---

## 🛠️ Technologies utilisées

- **C** — développement des clients et du serveur
- **Sockets réseau** — communication entre les différentes machines
- **Programmation client / serveur** — architecture générale du projet
- **Linux / terminal** — compilation et exécution des différentes versions

---

## 👥 Contexte du projet

Ce projet a été réalisé **en équipe de 4** durant le troisième semestre de BUT Informatique.

Le développement s'est déroulé du **8 au 17 décembre 2025**.

Ce projet m'a notamment permis de travailler sur :

- la programmation réseau en C ;
- la communication par sockets ;
- la conception d'une architecture client / serveur ;
- la gestion de plusieurs connexions ;
- la séparation des responsabilités entre serveur et clients ;
- la conception progressive d'une même application à travers plusieurs versions ;
- le débogage de problèmes liés aux communications réseau ;
- le travail en équipe sur une application distribuée.

---

## 📅 Réalisation

**Décembre 2025** — Deuxième année de BUT Informatique, semestre 3.
