package client;

import java.io.*;
import java.net.Socket;
import java.util.InputMismatchException;
import java.util.Scanner;

public class PlayerTCP implements Runnable{

    private Historique hist;
    private final String hostName;
    private final int port;
    private final String fileName;

    public PlayerTCP(String hostName, int port) {
        this.port = port;
        this.hostName = hostName;
        this.fileName = "";
    }

    public PlayerTCP(String hostName, int port, String fileName) {
        this.port = port;
        this.hostName = hostName;
        this.fileName = fileName;
    }

    @Override
    public void run() {
        try {
            Socket socket = new Socket(hostName, port);
            DataInputStream in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
            DataOutputStream ot = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));

            String pseudoClient = "";
            try (FileInputStream fis = new FileInputStream(this.fileName);
                 ObjectInputStream ois = new ObjectInputStream(fis)) {
                hist = (Historique) ois.readObject();
                pseudoClient = hist.getPseudo();
                System.out.println("Votre fichier a été chargé correctement.");
            } catch (IOException e) {
                System.out.println("Aucun fichier de sauvegarde valide n'a été choisi.");
                while (pseudoClient.length() != 8 || pseudoClient.matches(".*\\W+.*")) {
                    System.out.println("Veuillez saisir votre pseudo (8 charactères alphanumériques):");
                    pseudoClient = new Scanner(System.in).nextLine();
                }
                hist=new Historique();
                hist.setPseudo(pseudoClient);
            }

            MessagePlayer mp = new MessagePlayer(ot, in, pseudoClient);

            mp.initPlayer();

            String clientUDP = "-1";
            while (Integer.parseInt(clientUDP)>9999 || Integer.parseInt(clientUDP)<0) {
                System.out.println("Veuillez saisir votre port (4 chiffres):");
                try {
                    clientUDP = new Scanner(System.in).nextLine();
                } catch (InputMismatchException e) {
                    clientUDP = "-1";
                }
            }

            int UDPClient=Integer.parseInt(clientUDP);
            (new PlayerUDP(UDPClient)).run();

            PlayerMulticast multicastP=null;

            //user information has been registered correctly
            int status = 1;

            int idGame = -1;
            while (true) {
                if (status == 1) {
                    boolean flag;
                    int gameChoice = -1;
                    while (gameChoice != 0 && gameChoice != 1 && gameChoice != 2) {
                        System.out.println(
                                "\nCréer une partie [0]\n" +
                                        "Rejoindre une partie [1]\n" +
                                        "Réaliser une autre action [2]:"
                        );
                        try {
                            gameChoice = new Scanner(System.in).nextInt();
                        } catch (InputMismatchException e) {
                            gameChoice = -1;
                        }
                    }
                    if (gameChoice == 0) {
                        flag = mp.createGame(clientUDP);
                    } else if (gameChoice == 1) {
                        while (idGame < 0 || idGame > 255) {
                            System.out.println("Entrez le numéro de la partie que vous souhaitez rejoindre:");
                            try {
                                idGame = new Scanner(System.in).nextInt();
                            } catch (InputMismatchException e) {
                                idGame = -1;
                            }
                        }
                        flag = mp.joinGame(clientUDP, idGame);
                    } else {
                        flag = false;
                        status = execOtherCmd(ot, in, mp, 1);
                    }
                    if (flag) status++;
                }
                if (status == 2) {
                    boolean flag;
                    int startGame = -1;
                    while (startGame != 0 && startGame != 1) {
                        System.out.println(
                                "Valider votre inscription à la partie [0]\n" +
                                        "Réaliser une autre action [1]:"
                        );
                        try {
                            startGame = new Scanner(System.in).nextInt();
                        } catch (InputMismatchException e) {
                            startGame = -1;
                        }
                    }
                    if (startGame == 0) {
                        multicastP=mp.readyPlay();
                        multicastP.run();
                        flag = mp.getposIT();
                        if (flag) hist.newGame();
                    } else {
                        flag = false;
                        status = execOtherCmd(ot, in, mp, 2);//current step -> filter cmd
                    }
                    if (flag) status++;//partie commence
                }
                if (status == 3) {
                    boolean flag;
                    do {
                        int gameActn = -1;
                        while (gameActn != 0 && gameActn != 1 && gameActn != 2 && gameActn != 3 && gameActn != 4) {
                            System.out.println(
                                    "Abbandonner la partie [0]\n" +
                                            "Demander la liste des joueurs présents [1]\n" +
                                            "Envoyer un essage à tous les joueurs [2]\n" +
                                            "Envoyer un message à un seul joueur [3]\n" +
                                            "Réaliser un mouvement [4]:"
                            );
                            try {
                                gameActn = new Scanner(System.in).nextInt();
                            } catch (InputMismatchException e) {
                                gameActn = -1;
                            }
                        }
                        if (gameActn == 0) flag = mp.quitGame();
                        else if (gameActn == 1) flag = mp.listPlayerInGame(ot, in);
                        else if (gameActn == 4) {
                            boolean flagerror;
                            flag = false;
                            do {
                                System.out.println("Dans quelle direction souhaitez-vous vous déplacer [UPMOV, RIMOV, DOMOV ou LEMOV]?");
                                String direction = new Scanner(System.in).nextLine();
                                System.out.println("De combien de cases souhaitez-vous vous déplacer (nombre composé de 3 chiffres)?");
                                String distance = new Scanner(System.in).nextLine();
                                if (direction.equals("DOMOV") || direction.equals("UPMOV") || direction.equals("RIMOV") || direction.equals("LEMOV")) {
                                    try {
                                        if (distance.length() != 3 || Integer.parseInt(distance) > 999 || Integer.parseInt(distance) < 0) {
                                            flagerror = true;
                                        } else {
                                            flagerror = false;
                                            flag = mp.goMove(direction, distance);
                                        }
                                    } catch (NumberFormatException e) {
                                        flagerror = true;
                                    }
                                } else flagerror = true;
                            } while (flagerror);
                        } else {
                            String messagePlayer = "-";
                            while (messagePlayer.matches(".*\\W+.*")) {
                                System.out.println("Veillez entrez un message contenant des characters alphanumérique");
                                messagePlayer = new Scanner(System.in).nextLine();
                            }
                            if (gameActn == 3) {
                                String destPlayer = "-";
                                while (destPlayer.matches(".*\\W+.*")) {
                                    System.out.println("Veillez entrez un destinataire (characters alphanumérique)");
                                    messagePlayer = new Scanner(System.in).nextLine();
                                }
                                flag = mp.messOnePlayer(messagePlayer, destPlayer);
                            } else flag = mp.messAllPlayer(messagePlayer);
                        }
                    } while (!flag);
                    //fin de la partie
                    hist.endGame();
                    saveData();
                    socket.close();
                    if (multicastP!=null)multicastP.close();
                    //TODO: re connecter le client et ne pas break
                    status = 1;
                    break;
                }
            }
        }catch (Exception e) {
            System.out.println("Status Joueur: "+e);
            e.printStackTrace();
        }
    }

//-------------------------Aux Function---------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------


    public int execOtherCmd(DataOutputStream ot, DataInputStream in, MessagePlayer mp, int status) {
        String propositionCmdPlayer = "";
        if (status == 1 || status == 2) {
            //avant de choisir une partie
            propositionCmdPlayer += "Description de la partie [0]\n";
            propositionCmdPlayer += "Liste des joueurs inscrits [1]\n";
            propositionCmdPlayer += "Liste des parties en attentes [2]";
            if (status == 2) {
                //avant de lancer la partie (start):
                propositionCmdPlayer += "\nSe désinscrire de la partie [3]:";
            } else propositionCmdPlayer += ":";
        }
        int resOther = -1;
        while (resOther != 0 && resOther != 1 && resOther != 2 && resOther != 3) {
            System.out.println(propositionCmdPlayer);
            try {
                resOther = new Scanner(System.in).nextInt();
            } catch (InputMismatchException e) {
                resOther = -1;
            }
        }
        if (resOther == 0 || resOther == 1) {
            int nGame = -1;
            while (nGame < 0 || nGame > 255) {
                System.out.println("Veillez selectionner une partie (entre 0 et 255):");
                try {
                    nGame = new Scanner(System.in).nextInt();
                } catch (InputMismatchException e) {
                    nGame = -1;
                }
            }
            if (resOther == 1) mp.listPlayer(ot, in, nGame);
            else mp.sizeMaze(ot, in, nGame);
        } else if (resOther == 2) {
            mp.listGame(ot, in);
        } else {
            if (mp.unReadyPlay()) return 1;
        }
        return status;
    }

    public void saveData() {
        try (FileOutputStream fos = new FileOutputStream(("historique.ser"));
             ObjectOutputStream oos = new ObjectOutputStream(fos)) {
            oos.writeObject(hist);
        } catch (IOException e) {
            System.out.println("Impossible d'effectuer une sauvegarde.");
        }
    }
}