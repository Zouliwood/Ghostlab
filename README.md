# Ghostlab

**Équipe 60 :**
- Gabriel TEIXEIRA
- David ANDRAWOS SAAD

## Architecture
- Le client est développé en Java.

  Pendant son exécution, le client maintient 3 threads (pour les communications `UDP`, `TCP`, `Multicast`).

  La classe principale du client est nommée `Client`.

- En ce qui concerne le serveur, il est développé en C.

  Le serveur se compose d'un fichier principal qui accepte les connexions et crée un thread distinct pour chaque client.

  Les threads interagissent avec les composants du jeu en cours via des fonctions équipées de verrous, le verrou étant initialisé dans la fonction principale.

  Le port d'envoi UDP et l'adresse multicast sont créés lors du démarrage d'une partie et sont utilisés par les fonctions auxiliaires.

## Compilation
Pour compiler le projet, exécutez la commande `make` dans le terminal.

Utilisez `make clean` pour supprimer les fichiers `.class` et `server_exe`.

Utilisez `make history` pour nettoyer la sérialisation.

## Utilisation
Les commandes suivantes doivent être exécutées à partir de la racine du projet.

### 1 - Serveur
- Pour lancer le serveur, exécutez la commande `./server_exe [port]`.

  Le port par défaut est 6969, donc la commande `./serveur` suffit.

- Pendant l'utilisation, le serveur affiche les commandes entrantes.

- Pour arrêter le serveur, envoyez le signal d'interruption : `Ctrl+C`.

### 2 - Client
- Pour démarrer le client, exécutez la commande `java client.Client [adresse] [port] [fichier.ser]`.

  Ici, fournir des valeurs pour l'adresse et le port est obligatoire.
  
  Le troisième argument permet de charger un fichier sérialisé pour récupérer le nom d'utilisateur et le temps de jeu enregistrés.
  
  L'adresse est une adresse IP ou un nom de domaine.

- Pour interagir avec le serveur, sélectionnez les options affichées dans le terminal et suivez les instructions.

- Abandonner ou terminer le jeu sert à arrêter le client.

### 3 - Choix d'Implémentation
- Le démarrage d'une partie nécessite deux joueurs.
- L'implémentation des commandes de mouvement est basée sur l'exemple fourni dans l'énoncé. Cela signifie que `UPMOV` décrémente la valeur de `X`, et `DOMOV` l'incrémente. (`X` fait référence à la hauteur définie dans le protocole du labyrinthe)
