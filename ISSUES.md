# Limites et pistes d’amélioration

Ce document présente les limites actuelles de l’application ainsi que des propositions d’amélioration pour un fonctionnement plus robuste, portable et optimisé.

---

## 1. Gestion des erreurs (incomplète)

### Limite actuelle
- Les erreurs sont principalement gérées lors :
  - des manipulations de fichiers,
  - des allocations mémoire.
- La communication est souvent maintenue même après une erreur.
- Le contrôle de validité des données reçues n’intervient **qu’après l’échange**, ce qui peut entraîner des incohérences.

### Améliorations possibles
- Ajouter un **code d’erreur (`errno`) systématique dans chaque message échangé**.
- À la réception :
  - si `errno != 0` → fermeture immédiate de la connexion ;
  - sinon → poursuite de l’échange.
- Mettre en place une **fonction dédiée** à la gestion uniforme des erreurs (lecture de `errno`, envoi, fermeture propre).

---

## 2. Gestion des fichiers et mémoire (peu adaptée aux gros fichiers)

### Limite actuelle
- L’intégralité d’un fichier est chargée en mémoire avant envoi ou écriture.
- Cette approche n’est pas scalable et peut saturer la mémoire pour les fichiers volumineux.

### Améliorations possibles
- Transférer les fichiers **par blocs**, idéalement par paquets multiples de **1448 octets** (taille optimale d’envoi TCP).
- Créer des fonctions dédiées :
  - `ReadFile()` : lecture par blocs,
  - `WriteFile()` : écriture progressive avec modes `wb` puis `ab`,
  - `SizeFile()` : obtention indépendante de la taille d’un fichier.
- Éviter `fseek` répétés en réalisant la lecture dans une boucle interne unique.

---

## 3. Absence de gestion de l’endianess

### Limite actuelle
- Le client et le serveur sont supposés partager le même ordre d’octets.


## 4. Hypothèse sur la taille des entiers (`sizeof(int)`)

### Limite actuelle
- `int` est supposé avoir une taille constante (4 octets), ce qui n’est pas garanti sur toutes les architectures.

---

## 5. Absence de vérification de doublon lors du transfert

### Limite actuelle
- Si un fichier portant le même nom existe déjà à destination, il est écrasé **sans avertissement**.

### Améliorations possibles
- Avant écriture :
  - vérifier l’existence du fichier,
  - proposer : écraser / renommer automatiquement / refuser.

---

## 6. Répertoires de stockage non configurables

### Limite actuelle
- Les fichiers sont obligatoirement stockés dans le répertoire d’exécution :
  - côté serveur pour les fichiers téléversés,
  - côté client pour les fichiers téléchargés.

### Améliorations possibles
- Permettre la saisie d’un **chemin de destination**.
- Côté serveur : vérifier l’existence du chemin, retourner une erreur si invalide.
- Côté client : choix d’emplacement lors de la sauvegarde.

---

## 7. Gestion simplifiée du système de fichiers

### Limite actuelle
- Lister et manipuler les fichiers repose sur des structures rudimentaires.
- L’affichage et les messages sont peu lisibles.

### Améliorations possibles
- Utiliser des structures dynamiques (tableaux, listes) pour représenter les fichiers.
- Améliorer l’affichage (alignement, feedback plus clair).

---

## 8. Utilisation de `scanf` pour la saisie utilisateur

### Limite actuelle
- `scanf` augmente les risques :
  - dépassements de buffer,
  - caractères restants dans l’entrée standard.

### Améliorations possibles
- Remplacer `scanf` par `fgets`, plus sécurisé.
- Traiter ensuite la chaîne (ex. `sscanf` pour extraire les données).

---

## 9. Liste de fichiers incomplète côté client

### Limite actuelle
- Le client ne peut pas consulter l’intégralité des fichiers accessibles côté serveur.
- La vérification d’existence de fichier est fragile.

### Améliorations possibles
- Utiliser une exploration plus complète du répertoire côté serveur.
- Envoyer une liste complète au client en amont.

---

## 10. Envoi fragmenté de métadonnées

### Limite actuelle
- Les informations sont envoyées propriété par propriété, ce qui :
  - multiplie les paquets TCP,
  - augmente le remplissage inutile (padding).

---

## 11. Recherche de fichiers par sous-chaîne (comportement incorrect)

### Limite actuelle
- La recherche considère toute sous-chaîne comme correspondance valide  
  → `"test"` correspond aussi à `"latest.txt"`.

### Améliorations possibles
- Normaliser les noms (ex. ajout de délimiteurs).
- Encadrer les noms par espaces lors de la comparaison.
- Uniformiser cette méthode dans tout le code.

---