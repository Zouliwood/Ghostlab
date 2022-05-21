# Ghostlab

Groupe 60:
- Gabriel TEIXEIRA
- David SAAD

## Architecture
- Le client a été réalisé en JAVA.

  Le client possède durant son exécution 3 threads (pour l'UDP, TCP, Multicast).

  La classe principale du client ce nomme Client.

- Quant à lui, le serveur a été réalisé en C.

  Le serveur est composé d'un fichier principal qui accepte les connexions et créé un thread par client.

  Les threads interagissent avec les parties encours à l'aide de fonctions munies de lock, le verrou étant initialisé dans le main.

  Le port d'envoi UDP et l'adresse multicast sont créés à la création d'une partie et utilisés par les fonctions auxiliaires.

## Compilation
Pour compiler le projet, il suffit d'exécuter la commande `make` dans le terminal.

Utiliser `make clean` pour supprimer les `.class` et `server_exe`.

Utiliser `make history` pour clean la sérialisation.

## Utilisation
Les commandes suivantes sont à réaliser depuis la racine du projet.

### 1 - Le serveur
- Pour lancer le serveur il suffit d'exécuter la commande `./server_exe [port]`

    Le port par défaut est 6969, ainsi la commande ./serveur.

- Durant son utilisation le serveur affiche les commandes qu'il reçoit

- Pour interrompre le serveur il est nécessaire de lui envoyer un signal : `Ctrl+C`

### 2 - Le client
- Pour démarrer le client, il suffit d'exécuter la commande `java client.Client [adresse] [port] [fichier.ser]`

  Ici, il est obligatoire de rentrer des valeurs pour l'adresse et le port.
  
  Ce troisième argument permet de charger un fichier sérialisé et ainsi de récupérer le pseudo enregistré ainsi que son temps de jeu.
  
  L'adresse ici est une ip ou un nom de domaine.

- Pour interagir avec le serveur, il suffit de sélectionner des options affichées dans le terminal puis de suivre les instructions.

- Abandonner ou bien terminer la partie sont un moyen d'arrêter le client.

### 3 - Choix d'implémentation
- Pour commencer une partie deux joueurs sont requis.
- L'implémentation des commandes de mouvements est basée sur l'exemple du sujet, ce qui veut dire que UPMOV va décrémenter la valeur de X et DOMOV l'incrémenter.(X fait référence au protocole, hauteur du labyrinthe)