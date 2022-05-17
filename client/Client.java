package client;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.Scanner;

public class Client {

    public static void main(String[] args) {
        String pseudoClient = "";
        Historique hist;
        try (FileInputStream fis = new FileInputStream(args[2]);
             ObjectInputStream ois = new ObjectInputStream(fis)) {
            hist = (Historique) ois.readObject();
            pseudoClient = hist.getPseudo();
            System.out.println("Votre fichier a été chargé correctement.");
        } catch (Exception e) {
            System.out.println("Aucun fichier de sauvegarde valide n'a été choisi.");
            while (pseudoClient.length() != 8 || pseudoClient.matches(".*\\W+.*")) {
                System.out.println("Veuillez saisir votre pseudo (8 charactères alphanumériques):");
                pseudoClient = new Scanner(System.in).nextLine();
            }
            hist=new Historique();
            hist.setPseudo(pseudoClient);
        }

        String clientUDP = "-1";
        while (clientUDP.equals("-1")) {
            System.out.println("Veuillez saisir votre port (4 chiffres):");
            try {
                clientUDP = new Scanner(System.in).nextLine();
                if (Integer.parseInt(clientUDP)>9999 || Integer.parseInt(clientUDP)<1023) clientUDP="-1";
            } catch (Exception e) {
                clientUDP = "-1";
            }
        }

        int UDPClient=Integer.parseInt(clientUDP);

        if (args.length == 2 || args.length == 3) {
            try {
                new Thread((new PlayerTCP(args[0], Integer.parseInt(args[1]), pseudoClient, clientUDP, hist))).start();
                new Thread((new PlayerUDP(UDPClient))).start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else System.out.println(Error.nbrArgs);
    }

}