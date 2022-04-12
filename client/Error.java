package client;

public enum Error {
    nbrArgs ("Mauvais nombre d'argument en paramètre"),
    responseServ ("Mauvaise réponse de la part du serveur"),
    requestServ ("Impossible d'envoyer la requette au serveur"),
    valueInt ("La valeur doit être compriseentre 255 et 0");

    private final String nameError;

    Error(String nameError) {
        this.nameError = nameError;
    }
}