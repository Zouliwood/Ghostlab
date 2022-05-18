package client;

public enum Error {
    nbrArgs("Mauvais nombre d'arguments en paramètre."),
    responseServ("Mauvaise réponse de la part du serveur."),
    requestServ("Impossible d'envoyer la requête au serveur."),
    valueInt("La valeur doit être comprise entre 255 et 0."),
    requestClient("Votre requête est fausse.");

    private final String nameError;

    Error(String nameError) {
        this.nameError = nameError;
    }

    @Override
    public String toString() {
        return nameError;
    }
}