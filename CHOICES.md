#

Ce document fournit les choix qui ont été fait pour construire nos applications client/serveur.
Vous trouverez le squelette des codes dans README.


## 1. Objectifs

Notre application permet de transferer des fichiers d'un client vers un serveur et vice-versa. On souhaite : 

1. Envoyer un fichier du client au serveur. Pour cela, on veut fournir un nom de fichier existant dans notre arborescence, et l'envoyer au serveur. Une option possible serait de pouvoir choisir le répertoire de destination. Une autre option serait d'être averti si le fichier existe déjà, et demander si on veutl'écraser ou non (dans quel cas le transfer ne sera pas fait).

2. Envoyer un fichier du client au serveur. Pour cela, le serveur doit au minimum nous fournir la liste des fichiers qu'il détient dans son répertoire courant. Une option possible serait de nous fournir tous les fichiers existants dans l'arborescence descendante. De cette liste, le client choisi un nom de fichier. Le serveur doit alors envoyer le contenu de ce fichier au client qui le stockera dans un fichier avec le nom correct. Encore, on pourrait avoir une demande de confirmation si le fichier existe déjà.

3. Sortir de la connexion que quand l'utilisateur le veut : ne pas le faire par défaut à la fin des traitements de requêtes. Donc il faut demander à l'utilisateur si il veut continuer d'échanger des fichiers ou non. C'est au client de fermer la connexion.

Nous ne nous posons pas de contraintes de temps et de mémoire. L'application doit fonctionner pour plusieurs clientsqui se connectent à un serveur. 



## 2. Protocole choisi : TCP

Pour l'échange de données, nous avons deux choix : UDP ou TCP. Le premier est dit "sans connexion" c'est à dire qu'il ne fait pas de demande de connexion, et donc il n'a aucune gestion de perte de paquets ni de controle de flux. 

Cependant, c'est un protocole qui permet de faire des échanges en temps réel, même si il n'y a aucune garantie que ces échanges soient effectués, et soient vrais. A l'inverse, TCP permet de faire du controle de flux et de la gestion de perte de paquets, ce qui garantit plus fortement la bonne réception de l'information. En revanche, ce protocole n'est pas adapté pour les échanges en temps réel, car il requiert des réémissions quand les paquets sont perdus.

Dans notre cas, nous n'avons pas de contraintes de temps, mais des contraintes de qualité de service.
C'est pour cela que nous avons choisi TCP pour l'échange de nos données.



## 3. Définition client

Le client est la machine qui initialise la connexion. Après avoir configuré une socket donnée, il fait une demande de connexion à une machine serveur. Il envoie des requêtes au serveur pendant l'échange des données. L'échange de données se fait pendant une connexion seulement. C'est également lui qui termine la connexion en premier après avoir averti le serveur. Il peut avoir plusieurs clients qui se connectent sur le même serveur.



## 4. Définition serveur

Le serveur est la machine qui se place en écoute : sa socket est en attente de connexion. Après avoir accepté une connexion, il crée une nouvelle socket pour la connexion donnée pendant qu'il reste en écoute.
On suppose ici que l'on travaille sur un serveur parallèle : les requêtes sont traitées de manière parallèle sur le nombre de processeurs existants dans le serveur. Ainsi, le serveur peut être connecté à plusieurs clients à un temps donné. 



## 5. Hypothèses

1. Les fichiers ont des tailles petites : `< 2^32-1` pour stocker leur taille dans un int. Egalement, on décide de copier le contenu total du fichier dans une variable pour envoyer cette variable via TCP. Cela veut dire que la mémoire tient la taille du fichier + copie du fichier ouvert + variable avec le contenu. Ce choix n'est pas optimal et fait parti des améliorations à faire (ISSUES).

2. C'est le client qui initie le début et la fin de la connexion. Pour l'initialisation, étant donné que le 'h_accept' est blocant, il n'y a pas de problèmes. Par contre, la question se pose au niveau du serveur pour savoir si la connexion a été fermée ou non:
    - regarder dans netstat -... pour la correcte socket id et récupérer le status. Si le status est != ESTABLISHED alors la connexion doit être fermée. Soit ouvrir un processus et stocker le résultat de la commande dans une variable, ou stocker ce résultat dans un fichier qui peut être lu. Problème : le nom de ce fichier ne doit pas exister, et il doit être en lecture seulement.
    - regarder la commande socket status. Normalement, le status de la socket devrait être modifié avec la fermeture de la connexion. Encore, lire le résultat.
    - directement regarder dans les fichiers que netstat utilise pour afficher ses résultats.
    - envoyer régulièrement des messages au client pour s'assurer de leur bonne réception. Si il n'y a pas de messages d'erreur, alors la connexion existe encore.
    - supposer que le client envoie un message de fermeture avant sa terminaison de connexion. Si on reçoit ce code, alors il faut fermer la connexion.

3. On choisi de travailler avec la dernière option : le client envoie un message de terminaison de connexion. Cette option
s'accorde avec le fait que de toute façon l'utilisateur à l'interface client fait trois choix : 
    - (1) envoyer un ficher,
    - (2) récuperer un fichier, 
    - (3) sortir.

4. On suppose que toutes les machines ont les même taille d'entier INT, le même boutisme, et les mêmes codes d'erreur errno : on ne se préocupe pas de problèmes de compatibilité entre les machines.

5. On utilise fork() pour permettre au serveur d'être connecté à plusieurs clients en même temps.

Note : les h_read sont blocants.


## 6.Choix

Des deux côtés client/serveur, les échanges sont divisés en trois parties : 
1. L'échange d'un fichier client -> serveur
2. L'échange d'un fichier serveur -> client
3. La sortie de la connexion.

Ce choix est déterminé par l'utilisateur au niveau de l'interface client. Il est systématiquement envoyé au serveur à chaque début d'échange pour déterminer quel comportement le serveur doit avoir pour les prochains échanges. La connexion est conservée tant que le client n'a pas fait une demande de terminaison, c'est à dire qu'il a soit mal tapé son choix, soit fait le choix 3.

Au niveau de l'envoi d'entiers, nous avons décidé de ne pas convertir le nombre en string (sinon on aurait des tables conséquentes pour les fichiers lourds), mais de stocker la valeur hexadécimale de l'entier dans un tableau de 5 char (5ième pour le charactère de fin de chaine '\0'). Comme ça, le transfer de cette donnée se fait en n'utilisant que 5 octets, au lieu d'autant que de chiffres si on avait fait une convertion int <-> string en utilisant les charactères ASCII.

Au niveau des erreurs : on se concentre majoritairement sur les erreurs liées aux manipulations de fichiers et d'allocation de mémoire. On évite à tout prix d'avoir le serveur qui s'arrête ou ferme la connexion par lui même. Ainsi, la pertinence des informations reçues/envoyées dépend du code d'erreur final envoyé. Ce code d'erreur correspond à la dernière erreur rencontrée au niveau du serveur.
Du côté client, si les erreurs sont faites avant l'envoi du choix, alors on sort du switch et on re-demande le choix. Si le choix a été envoyé, mais que la prochaine information lue par le serveur est le choix, alors on sort du switch, et on demande à l'utilisateur de fermer la connexion. Si il ne le fait pas, il n'y a aucune garantie que le code fonctionne (ce serait extrêment invraisemblable).
Fort heureusement, on se trouve toujours dans ces deux cas. Le fait que l'on continue l'échange malgré le fait qu'il y a eu une erreur n'est pas optimisé. Certains cas d'erreur codées n'ont pas été testés.
