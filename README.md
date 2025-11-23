## 📁 Application Client/Serveur de Transfert de Fichiers

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
├── INSTALL.md      # instructions de compilation / installation détaillées
├── CHOICES.md      # choix techniques détaillés
└── ISSUES.md       # pistes d’amélioration / bugs connus

```
---

## 5. Compilation

Depuis le dossier ``` src ``` :

```bash

cd src
make
# ou gmake si nécessaire

```

Les exécutables générés peuvent ensuite être appelés depuis ``` client/ ``` et ``` server/ ```.

--- 

## 6. Utilisation

### 6.1 Lancer le serveur

```bash

cd server
./serveur

```

### 6.2 Lancer le serveur

Dans un autre terminal : 

```bash

cd client
./client

```

Le client affiche alors un menu :
   1. envoyer un fichier au serveur ;
   2. récupérer un fichier depuis le serveur ;
   3. quitter (fermeture de la connexion).

---

## 7. Squelette du code & Structure des échanges

Vous trouverez ci-dessous le squelette des deux codes client/serveur avec leur échanges respectifs.

```text

-------------------------------------------------------------------------------------------------------------------------------------------
CLIENT                                                              SERVEUR
-------------------------------------------------------------------------------------------------------------------------------------------
création d'une socket                                               création d'une socket
                                                                    configuration de la socket
                                                                    mise en écoute de la socket
                                                                    attente d'une demande de connexion en boucle infty
connexion et configuration de la socket                    >>>>>    acceptation de la connexion avec création d'une nouvelle socket.
-------------------------------------------------------------------------------------------------------------------------------------------
Echange de données :                                                Echange de données :
choix = 0                                                           choix = 0
tant que (choix != 3)                                               tant que (choix !=3)
start tantque                                                       start tantque
choix : 1. envoyer fichier client -> serveur                        choix = socket_lire choix envoyé par le client
        2. envoyer fichier serveur -> client
        3. sortir.
switch (choix)                                                      switch (choix)
*******************************************************************************************************************************************
Choix 1 :                                                           Choix 1 : 
récupérer nom du fichier                                            
récupérer contenu du fichier et taille du fichier
si erreur alors sortir du switch                   
socket_envoyer choix + taille + nom + contenu               >>>>>   socket_lire taille + nom + contenu du fichier
liberer contenu                                                     ecrire contenu dans un fichier <nom>
imprimer erreurs possibles côté serveur                     <<<<<   envoyer code d'erreur
*******************************************************************************************************************************************
Choix 2 :                                                           Choix 2 : 
socket_envoyer choix                                        >>>>>        
socket_lire fichiers dans répertoire courant du serveur     <<<<<   socket_envoyer fichiers dans le répertoire courant. Si erreur, envoyer
                                                                    une chaine vide.
choisir un nom de fichier existant. Aucun choix n'est
possible si on a reçu précédemment une chaine vide. On doit alors
arrêter le code avec Ctrl+C sans prévenir le serveur.        
socket_envoyer le nom du fichier                            >>>>>   socket_lire le nom du fichier
                                                                    récuperer taille du fichier (si erreur taille = 1)
                                                                    récuperer contenu (si erreur contenu = "")
socket_lire taille + contenu du fichier                     <<<<<   socket_envoyer taille + contenu
                                                                    liberer contenu
imprimer erreurs possibles côté serveur                     <<<<<   envoyer code d'erreur (permet de savoir si contenu envoyé pertinant ou non)
Si pas d'erreur, ecrire contenu dans fichier de nom choisi                                                             
*******************************************************************************************************************************************
Choix 3 :                                                           Choix 3 : 
socket_envoyer choix                                        >>>>>   On sort naturellement avec la condition tantque
*******************************************************************************************************************************************
Choix defaut :                                                      Choix defaut : 
socket_envoyer choix = 3                                    >>>>>   Normalement ne devrait pas exister. Au cas où, choix = 3
*******************************************************************************************************************************************
endtantque                                                          endtantque
-------------------------------------------------------------------------------------------------------------------------------------------
Fermer la connexion                                                 Fermer la connexion
-------------------------------------------------------------------------------------------------------------------------------------------
                                                                    Attendre une nouvelle connexion
-------------------------------------------------------------------------------------------------------------------------------------------

```

--- 

## 8. Améliorations possibles

- Gestion de très gros fichiers (envoi/lecture par blocs, sans tout charger en mémoire).
- Meilleure portabilité (gestion du boutisme, types entiers normalisés).
- Interface utilisateur plus claire (messages, confirmations d’écrasement, etc.).
- Gestion plus fine des erreurs et des cas limites côté serveur comme côté client.







