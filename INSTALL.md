# Installation & Compilation

## Organisation des fichiers compilés

Les fichiers objets ainsi que les exécutables sont conservés **dans le dossier `src/`** (pas de dossier `obj/` séparé).  
Ce choix permet de garder une structure simple pour ce projet de petite envergure.

---

## 1. Cloner le dépot

Commence par récupérer le projet en local avec `git clone` :

```bash
git clone https://github.com/m2cci-bartetj/FileTransferApp.git

```


##  2. Compilation

Depuis la racine du projet, exécute :

```bash
cd src
make
```


## 3. Utilisation

⚠️ Attention à bien exécuter le client et le serveur depuis leur dossier respectif : `/server` et `/client`

### 3.1 Lancer le serveur

```bash

cd server
./serveur

```

### 3.2 Lancer le serveur

Dans un autre terminal : 

```bash

cd client
./client

```

Le client affiche alors un menu :
   1. envoyer un fichier au serveur ;
   2. récupérer un fichier depuis le serveur ;
   3. quitter (fermeture de la connexion).

