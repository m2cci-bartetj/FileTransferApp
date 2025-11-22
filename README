# Application Client/Serveur de Transfert de Fichiers

Ce projet implémente une application **client/serveur de transfert de fichiers** au-dessus de **TCP**.  
L’objectif est de permettre à plusieurs clients de se connecter à un serveur pour **envoyer** ou **récupérer** des fichiers, en respectant les notions vues en cours (sockets, TCP, gestion de connexion, fork…).

---

## 1. Objectifs du projet

L’application permet, pour un client connecté à un serveur :

1. **Envoyer un fichier du client vers le serveur**
   - Le client fournit le nom d’un fichier existant dans son arborescence.
   - Le contenu est transféré au serveur, qui le stocke dans son répertoire courant.
   - Améliorations possibles : choix du répertoire de destination, confirmation en cas d’écrasement d’un fichier déjà existant.

2. **Récupérer un fichier du serveur vers le client**
   - Le serveur fournit la liste des fichiers présents dans son répertoire courant.
   - Le client choisit un fichier à télécharger.
   - Le fichier est envoyé par le serveur et sauvegardé côté client sous le même nom.
   - Améliorations possibles : confirmation si un fichier du même nom existe déjà côté client.

3. **Garder la connexion ouverte tant que l’utilisateur le souhaite**
   - Le client peut enchaîner plusieurs envois/réceptions de fichiers.
   - La connexion n’est fermée que lorsque le client choisit explicitement de quitter.

L’application peut fonctionner avec **plusieurs clients** connectés au même serveur (serveur parallèle via `fork()`).

---

## 2. Protocole et choix techniques

### 2.1. Protocole de transport

Nous utilisons **TCP**, car :
- UDP est sans connexion et ne garantit pas la livraison des paquets,
- TCP fournit contrôle de flux, reprise en cas de pertes et fiabilité,
- Pour un transfert de fichiers, la priorité est l’intégrité des données, pas le temps réel.

### 2.2. Rôle du client

- Initialise la connexion vers le serveur.
- Propose un menu à l’utilisateur :
  1. envoyer un fichier,
  2. récupérer un fichier,
  3. quitter.
- Envoie le choix et les données associées (noms de fichiers, contenus…).
- Ferme la connexion en dernier, après avoir envoyé la demande de fermeture.

### 2.3. Rôle du serveur

- Crée une socket, la configure et la met en écoute.
- Accepte les connexions entrantes en boucle.
- Pour chaque client : `accept()` puis `fork()` pour traiter le client en parallèle.
- Traite les requêtes (envoi/réception de fichiers) jusqu’à la demande de fermeture.

---

## 3. Hypothèses et limites

- Les fichiers sont considérés comme de **taille « raisonnable »** (taille stockée dans un `int`) et sont chargés en mémoire avant envoi.
- Le **client** initie le début et la fin de la connexion.
- On suppose que les machines ont la même représentation des `int` et le même boutisme.
- Les lectures sur la socket sont bloquantes.
- Certains cas d’erreur rares ne sont pas gérés/exhaustivement testés.

---

## 4. Organisation du dépôt

```text
.
├── src/         # sources C du client et du serveur (+ makefile)
│   ├── client.c
│   ├── serveur.c
│   ├── Makefile
│   └── …
├── client/      # répertoire de travail du client (fichiers à envoyer / reçus)
├── server/      # répertoire de travail du serveur (fichiers stockés côté serveur)
├── README.md    # documentation principale
├── INSTALL      # instructions de compilation / installation détaillées
├── CHOICES      # choix techniques détaillés
└── ISSUES       # pistes d’amélioration / bugs connus
