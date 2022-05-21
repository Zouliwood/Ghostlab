# Ghostlab

Groupe 60:
- Gabriel TEIXEIRA
- David SAAD

## Architecture
- Le client a été réalisé en JAVA.

  Le client possède durant son exécution 3 threads (pour l'UDP, TCP, Multicast).

  La classe principale du client ce nome Client.

- Quant à lui, le serveur a été réalisé en C.

  Le serveur est composé d'un fichier principal qui accepte les connexions et crée un thread par client.

  Les threads interagissent avec les parties encours à l'aide de fonctions munies de lock, le verrou étant initialisé dans la main.

  Le port d'envoi UDP et l'adresse multicast sont créer à la création d'une partie et utilisé par les fonctions auxiliaires.

## Compilation
Pour compiler le projet, il suffit d'exécuter la commande make dans le terminal.

## Utilisation
Les commandes suivantes sont à réaliser depuis la racine du projet.

### 1 - Le serveur
- Pour lancer le serveur il suffit d'exécuter la commande `./server_exe [port]`

    Le port par défaut est `6969`, ainsi la commande `./serveur.

- Durant sont utilisation le serveur affiche les commandes qu'ils recoit

- Pour interompre le serveur il est nécessaire de lui envoyer un signal : `Ctrl+C`

### 2 - Le client
- Pour démarrer le client, il suffit d'exécuter la commande `./java client.Client [adresse] [port]`

  Ici, il est obligatoire de rentrer des valeurs pour l'adresse et le port.
    
  L'adresse ici est une ip ou un nom de domaine.

- Pour interagir avec le serveur, il suffit de sélectionner des options affichées dans le terminal puis de suivre les instructions.

- Abandonner ou bien terminer la partie sont un moyen d'arrêter le serveur.

### 3 - Choix d'implémentation
- Pour commencer une partie deux joueurs sont requis.
- L'implémentation des commandes de mouvements sont basées sur l'exemple du sujet, ce qui veut dire que UPMOV va décrémenter la valeur de X et DOMOV l'incrémenter.(X fait réfèrence au protocole,hauteur du labyrinthe) 