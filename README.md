# Ghostlab

Groupe 60:
- Gabriel TEIXEIRA
- David SAAD

## Architecture
- Le client a été réalisé en JAVA.

    Le client possède durant sont exécution 3 threads (pour l'UDP, TCP, Multicast).

    La classe principale du client se nome Client.


- Quant a lui le serveur a été réaliser en C.

    Le serveur est composé d'un main qui accepte les connexions et crée un thread par client.
    Les threads interagissent avec les parties encours à l'aide de fonctions muni de lock, le verrou étant initialisé dans le main.
    Le port d'envoie UDP et l'addresse multicast sont créer à la creation d'une partie et utilisé par les fonctions auxiliaire.

## Compilation
Pour compiler le projet il suffit d'exécuter la commande make dans le terminal.

## Utilisation
Les commandes suivantes sont a réaliser depuis la racine du projet.

### 1 - Le serveur
- Pour lancer le serveur il suffit d'exécuter la commande `./server_exe [port]`

    Le port par défaut est `6969`, ainsi la commande `./serveur.

- Durant sont utilisation le serveur affiche les commandes qu'ils recoit

- Pour interompre le serveur il est nécessaire de lui envoyer un signal : `Ctrl+C`
### 2 - Le client
- Pour démarrer le client il suffit d'exécuter la commande `./java client.Client [adresse] [port]`

    Ici il est obligatoire de rentrer des valeurs pour l'adresse et le port.
    L'adresse ici est une ip ou un nom de domaine.

- //selection des options

- Abandonner ou bien terminer la partie est un moyen d'arrêter le serveur.
