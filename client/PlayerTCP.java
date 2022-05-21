package client;

import java.io.*;
import java.net.Socket;
import java.util.InputMismatchException;
import java.util.Scanner;

public class PlayerTCP implements Runnable{

    private final Historique hist;
    private final String hostName;
    private final int port;
    private final String pseudoClient;
    private String clientUDP;

    public PlayerTCP(String hostName, int port, String pseudoClient, String clientUDP, Historique hist) {
        this.port = port;
        this.hostName = hostName;
        this.pseudoClient=pseudoClient;
        this.clientUDP=clientUDP;
        this.hist=hist;
    }

    @Override
    public void run() {
        try {
            Socket socket = new Socket(hostName, port);
            DataInputStream in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
            DataOutputStream ot = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));

            int UDPClient=Integer.parseInt(clientUDP);
            PlayerUDP pUDP=null;
            while (pUDP==null){
                try{
                    pUDP=new PlayerUDP(UDPClient);
                }catch (Exception e){
                    System.out.println("Le port est indisponible.");
                    clientUDP = "-1";
                    while (clientUDP.equals("-1")) {
                        System.out.println("Veuillez saisir votre port (4 chiffres):");
                        try {
                            clientUDP = new Scanner(System.in).nextLine();
                            if (Integer.parseInt(clientUDP)>9999 || Integer.parseInt(clientUDP)<1023) clientUDP="-1";
                        } catch (Exception ingnored) {
                            clientUDP = "-1";
                        }
                    }
                    UDPClient=Integer.parseInt(clientUDP);
                }
            }

            //user information has been registered correctly
            MessagePlayer mp = new MessagePlayer(ot, in, pseudoClient);
            mp.initPlayer();

            Thread udpT = new Thread(pUDP);
            udpT.start();

            Thread multiT=null;
            PlayerMulticast multicastP=null;
            Plateau platPlayer=null;

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
                            System.out.println("Entrez le numéro de la partie que vous souhaitez rejoindre (de 0 à 255):");
                            try {
                                idGame = new Scanner(System.in).nextInt();
                            } catch (InputMismatchException e) {
                                idGame = -1;
                            }
                        }
                        flag = mp.joinGame(clientUDP, idGame);
                        idGame=-1;//refaire une boucle en cas d'erreur
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
                        //get dimension map
                        String[] arrRespString=mp.readyPlay();
                        multicastP= new PlayerMulticast(arrRespString[0], Integer.parseInt(arrRespString[1]));
                        multiT=new Thread(multicastP);
                        multiT.start();
                        //get pos player
                        String[] arrResponsePosString = mp.getposIT();
                        flag = Boolean.parseBoolean(arrResponsePosString[0]);
                        if (flag) {
                            hist.newGame();
                            int widthmap=Integer.parseInt(arrRespString[2]);
                            int heightmap=Integer.parseInt(arrRespString[3]);
                            int posxplayer=Integer.parseInt(arrResponsePosString[1]);
                            int posyplayer=Integer.parseInt(arrResponsePosString[2]);
                            //init map player
                            platPlayer=new Plateau(heightmap, widthmap, posxplayer, posyplayer);
                        }
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
                                    "Abandonner la partie [0]\n" +
                                            "Demander la liste des joueurs présents [1]\n" +
                                            "Envoyer un message à tous les joueurs [2]\n" +
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
                                //print player map
                                if (platPlayer!=null)platPlayer.printPlateau();
                                System.out.println("Dans quelle direction souhaitez-vous vous déplacer :\n" +
                                        "Vers le haut [0]\n" +
                                        "Vers la droite [1]\n" +
                                        "Vers le bas [2]\n" +
                                        "Vers la gauche [3]");
                                int direction = new Scanner(System.in).nextInt();
                                System.out.println("De combien de cases souhaitez-vous vous déplacer (entre 0 et 999)?");
                                int distance = new Scanner(System.in).nextInt();
                                if (direction >= 0 && direction <= 3) {
                                    try {
                                        if (distance > 999 || distance < 0) {
                                            flagerror = true;
                                        } else {
                                            flagerror = false;
                                            String distanceStr = String.format("%03d", distance);
                                            String[] respDirectionString = mp.goMove(direction, distanceStr);
                                            flag = Boolean.parseBoolean(respDirectionString[0]);
                                            //update mov player map
                                            if (respDirectionString.length==3 && platPlayer!=null){
                                                platPlayer.addWall(direction, distance,
                                                        Integer.parseInt(respDirectionString[1]),
                                                        Integer.parseInt(respDirectionString[2])
                                                );
                                            }
                                        }
                                    } catch (NumberFormatException e) {
                                        flagerror = true;
                                    }
                                } else flagerror = true;
                            } while (flagerror);
                        } else {
                            String messagePlayer = "*+";
                            while ((messagePlayer.length() > 200 || messagePlayer.length() <=0)
                                    || messagePlayer.contains("*") || messagePlayer.contains("+")) {
                                System.out.println("Veuillez entrer un message contenant des caractères (différent de * et +):");
                                messagePlayer = new Scanner(System.in).nextLine();
                            }
                            if (gameActn == 3) {
                                String destPlayer = "-";
                                while (destPlayer.length() != 8 || destPlayer.matches(".*\\W+.*")) {
                                    System.out.println("Veuillez entrer un destinataire (8 caractères alphanumériques).");
                                    destPlayer = new Scanner(System.in).nextLine();
                                }
                                flag = mp.messOnePlayer(messagePlayer, destPlayer);
                            } else flag = mp.messAllPlayer(messagePlayer);
                        }
                    } while (!flag);
                    endGame(socket, multicastP, multiT, pUDP, udpT);
                    //TODO: connect the client again
                    status = 1;
                    break;
                }
            }
        }catch (Exception e) {
            System.out.println("Une erreur est survenue.");
        }
    }

//-------------------------Aux Function---------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------


    public void endGame(Socket socket, PlayerMulticast multicastP, Thread multiT, PlayerUDP pUDP, Thread udpT) throws Exception {
        //fin de la partie
        //save data
        hist.endGame();
        saveData();

        //close tcp
        socket.close();
        System.out.println("Fin TCP.");

        //close multicast
        if (multicastP != null){
            multicastP.close();
            multiT.interrupt();
        }

        //close udp
        pUDP.close();
        udpT.interrupt();
    }

    public int execOtherCmd(DataOutputStream ot, DataInputStream in, MessagePlayer mp, int status) {
        String propositionCmdPlayer = "";
        if (status == 1 || status == 2) {
            //avant de choisir une partie
            propositionCmdPlayer += "Description de la partie [0]\n";
            propositionCmdPlayer += "Liste des joueurs inscrits [1]\n";
            propositionCmdPlayer += "Liste des parties en attente [2]";
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
                System.out.println("Veuillez sélectionner une partie (entre 0 et 255):");
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
        try (FileOutputStream fos = new FileOutputStream(("historique_"+this.pseudoClient+".ser"));
             ObjectOutputStream oos = new ObjectOutputStream(fos)) {
            oos.writeObject(hist);
        } catch (IOException e) {
            System.out.println("Impossible d'effectuer une sauvegarde.");
        }
    }
}