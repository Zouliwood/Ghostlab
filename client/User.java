package client;

import java.io.*;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

public class User {

    private static int status=0;

    static class ConnectServerTCP implements Runnable {

        private final String hostName;
        private final int port;

        public ConnectServerTCP(String hostName, int port){
            this.port=port;
            this.hostName=hostName;

        }

        public String readInput(DataInputStream in, int size) throws IOException {
            byte[] input_header=new byte[size];
            int result=in.read(input_header, 0, size);
            if (result==-1){
                System.out.println(Error.responseServ);
                throw new IOException();
            }
            return new String(input_header, StandardCharsets.UTF_8);
        }

        public void execOtherCmd(DataOutputStream ot, DataInputStream in, Message m, MessagePlayer mp, int status){
            String propositionCmdPlayer="";
            if (status==1 || status==2){
                //avant de choisir une partie
                propositionCmdPlayer+="Description de la partie [0]\n";
                propositionCmdPlayer+="Liste des joueurs inscrits [1]\n";
                propositionCmdPlayer+="Liste des parties en attentes [2]";
                if(status==2){
                    //avant de lancer la partie (start):
                    propositionCmdPlayer+="\nSe désinscrire de la partie [3]:";
                }else propositionCmdPlayer+=":";
            }
            int resOther=-1;
            while (resOther!=0 && resOther!=1 && resOther!=2 && resOther!=3){
                System.out.println(propositionCmdPlayer);
                resOther=new Scanner(System.in).nextInt();
            }
            if (resOther==0 || resOther==1){
                int nGame=-1;
                while (nGame<0 || nGame>255){
                    System.out.println("Veillez selectionner une partie (entre 0 et 255):");
                    nGame=new Scanner(System.in).nextInt();
                }
                if (resOther==1) m.listPlayer(ot, in, nGame);
                else m.sizeMaze(ot, in, nGame);
            } else if (resOther==2) {
                m.listGame(ot, in);
            } else {
                if (mp.unReadyPlay()) status=1;
            }
        }

        @Override
        public void run() {
            try {
                Socket socket = new Socket(hostName, port);
                DataInputStream in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
                DataOutputStream ot = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));

                //[GAME n***]
                int nbrPlayersWaiting=0;
                if (readInput(in, 6).equals("GAMES ")){
                    nbrPlayersWaiting=in.readUnsignedByte() & 0xFF;
                    System.out.format("Il y a %d parties en attente.\n",nbrPlayersWaiting);
                    if (!readInput(in, 3).equals("***")){
                        System.out.println(Error.responseServ);
                        throw new IOException();
                    }
                }else{
                    System.out.println(Error.responseServ);
                    throw new IOException();
                }

                //[OGAME m s***]
                for (int i = 0; i < nbrPlayersWaiting; i++) {
                    if (readInput(in, 6).equals("OGAME ")){
                        int nbrGame=in.readUnsignedByte() & 0xFF;
                        readInput(in,1);
                        int nbrPlayersRegistred=in.readUnsignedByte() & 0xFF;
                        System.out.format("La partie numéro %d a %d joueurs dans sa salle d'attente.\n", nbrGame, nbrPlayersRegistred);
                        if (!readInput(in, 3).equals("***")){
                            System.out.println(Error.responseServ);
                            throw new IOException();
                        }
                    }else{
                        System.out.println(Error.responseServ);
                        throw new IOException();
                    }
                }

                String pseudoClient = "";
                while (pseudoClient.length()!=8 && pseudoClient.matches("^[a-zA-Z0-9]*$")){
                    System.out.println("Veuillez saisir votre pseudo (8 charactères alphanumériques):");
                    pseudoClient=new Scanner(System.in).nextLine();
                }

                int clientUDP=0;
                while (clientUDP<1000 || clientUDP>9999){
                    System.out.println("Veuillez saisir votre port (4 chiffres):");
                    clientUDP = new Scanner(System.in).nextInt();
                }


                Message m=new Message(ot, in, pseudoClient, clientUDP);
                MessagePlayer mp=new MessagePlayer(ot, in, pseudoClient, clientUDP);
                Player p=new Player(clientUDP);

                //user information has been registered correctly
                User.status=1;

                int idGame = -1;
                while (true){
                    if (status==1){
                        boolean flag;
                        int gameChoice=-1;
                        while (gameChoice!=0 && gameChoice!=1 && gameChoice!=2){
                            System.out.println(
                                    "\nCréer une partie [0]\n"+
                                    "Rejoindre une partie [1]\n"+
                                    "Réaliser une autre action [2]:"
                            );
                            gameChoice=new Scanner(System.in).nextInt();
                        }
                        if (gameChoice == 0) {
                            flag=m.createGame(clientUDP);
                        } else if(gameChoice==1){
                            while (idGame < 0 || idGame > 255) {
                                System.out.println("Entrez le numéro de la partie que vous souhaitez rejoindre:");
                                idGame = new Scanner(System.in).nextInt();
                            }
                            flag=m.joinGame(clientUDP, idGame);
                        }else{
                            flag=false;
                            execOtherCmd(ot, in, m, mp, 1);
                        }
                        if (flag)status++;
                    }
                    if (status==2){
                        boolean flag;
                        int startGame=-1;
                        while (startGame!=0 && startGame!=1){
                            System.out.println(
                                    "Valider votre inscription à la partie [0]\n"+
                                    "Réaliser une autre action [1]:"
                            );
                            startGame=new Scanner(System.in).nextInt();
                        }
                        if (startGame==0) flag=mp.readyPlay();
                        else{
                            flag=false;
                            execOtherCmd(ot, in, m, mp, 2);//current step -> filter cmd
                        }
                        if (flag)status++;//partie commence
                    }
                    if (status==3){
                        boolean flag;
                        do{
                            int gameActn=-1;
                            while (gameActn!=0 && gameActn!=1 && gameActn!=2 && gameActn!=3 && gameActn!=4){
                                System.out.println(
                                        "Abbandonner la partie [0]\n"+
                                        "Demander la liste des joueurs présents [1]\n"+
                                        "Envoyer un essage à tous les joueurs [2]\n"+
                                        "Envoyer un message à un seul joueur [3]\n"+
                                        "Réaliser un mouvement [4]:"
                                );
                                gameActn=new Scanner(System.in).nextInt();
                            }
                            if (gameActn==0) flag=mp.quitGame();
                            else if(gameActn==1) flag=mp.listPlayer(ot, in, idGame);
                            else if(gameActn==4){
                                boolean flagerror;
                                flag=false;
                                do{
                                    System.out.println("Dans quelle direction souhaitez-vous vous déplacer [UPMOM, RIMOV, DOMOV ou LEMOV]?");
                                    String direction=new Scanner(System.in).nextLine();
                                    System.out.println("De combien de cases souhaitez-vous vous déplacer (nombre composé de 3 chiffres)?");
                                    String distance=new Scanner(System.in).nextLine();
                                    if (direction.equals("DOMOV") || direction.equals("UPMOV") || direction.equals("RIMOV") ||  direction.equals("LEMOV")) {
                                        if (distance.length()!=3 || Integer.parseInt(distance)>999 || Integer.parseInt(distance)<0){
                                            flagerror = false;
                                            flag=mp.goMove(direction, distance);
                                        }else flagerror=true;
                                    }else flagerror=true;
                                }while (flagerror);
                            }else{
                                String messagePlayer="-";
                                while (!messagePlayer.matches("^[a-zA-Z0-9]*$")){
                                    System.out.println("Veillez entrez un message contenant des characters alphanumérique");
                                    messagePlayer=new Scanner(System.in).nextLine();
                                }
                                if (gameActn==3){
                                    String destPlayer="-";
                                    while (!destPlayer.matches("^[a-zA-Z0-9]*$")){
                                        System.out.println("Veillez entrez un destinataire (characters alphanumérique)");
                                        messagePlayer=new Scanner(System.in).nextLine();
                                    }
                                    flag=mp.messOnePlayer(messagePlayer, destPlayer);
                                }else flag=mp.messAllPlayer(messagePlayer);
                            }
                        }while (!flag);
                        status=1;
                    }
                }
            } catch (Exception e) {
                System.out.println(e);
                e.printStackTrace();
            }
        }
    }

    private static class Message implements MessageUserPlayer{

        DataOutputStream ot;
        DataInputStream in;
        int clientUDP;
        String pseudoClient;

        public Message(DataOutputStream ot, DataInputStream in, String pseudoClient, int clientUDP){
            this.ot=ot;
            this.in=in;
            this.clientUDP=clientUDP;
            this.pseudoClient=pseudoClient;
        }

        //[NEWPL id port***]
        public boolean createGame(int UDPPlayer) {
            try{
                //request
                String s="NEWPL "+this.pseudoClient+" port"+END_TCP;
                byte[] request=s.getBytes();
                byte[]udpplayer=ByteBuffer.allocate(4).putInt(UDPPlayer).array();
                System.arraycopy(udpplayer, 0, request, 15, 4);
                this.ot.write(request);
                this.ot.flush();
                //response
                int idGame;
                byte[] response=readFirstMessage(this.in);
                String responseString=new String(response, StandardCharsets.UTF_8);
                if (responseString.startsWith("REGOK")){
                    idGame=response[6];
                    System.out.println("Vous êtes maintenant inscrit dans la partie "+idGame);
                    return true;
                }else if(responseString.startsWith("REGNO"))System.out.println("La partie n'a pas pu être créé");
                else System.out.println(Error.responseServ);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.valueInt);
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
            return false;
        }

        //[REGIS id port m***]
        public boolean joinGame(int UDPPlayer, int idGame){
            try{
                //request
                String s="REGIS "+this.pseudoClient+" port m"+END_TCP;
                byte[] request=s.getBytes();
                inRangeUint8(idGame);
                request[20]=(byte)idGame;
                byte[]udpplayer=ByteBuffer.allocate(4).putInt(UDPPlayer).array();
                System.arraycopy(udpplayer, 0, request, 15, 4);
                this.ot.write(request);
                this.ot.flush();
                //response
                int idGameJ;
                byte[] response=readFirstMessage(this.in);
                String responseString=new String(response, StandardCharsets.UTF_8);
                if (responseString.startsWith("REGOK")){
                    idGameJ=response[6];
                    System.out.println("Vous êtes maintenant inscrit dans la partie "+idGameJ);
                    return true;
                }else if(responseString.startsWith("REGNO"))System.out.println("Vous n'avez pas pu rejoindre la partie");
                else System.out.println(Error.responseServ);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.valueInt);
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
            return false;
        }

    }

    protected static class MessagePlayer implements MessageUserPlayer{

        DataOutputStream ot;
        DataInputStream in;
        int clientUDP;
        String pseudoClient;

        public MessagePlayer(DataOutputStream ot, DataInputStream in, String pseudoClient, int clientUDP){
            this.ot=ot;
            this.in=in;
            this.clientUDP=clientUDP;
            this.pseudoClient=pseudoClient;
        }

        //[START***]
        public boolean readyPlay(){
            try{
                //request
                String s="START"+END_TCP;
                byte[] request=s.getBytes();
                this.ot.write(request);
                this.ot.flush();
                //response
                byte[] response=readFirstMessage(in);
                String responseString=new String(response, StandardCharsets.UTF_8);
                //[WELCO m h w f ip port***]
                if(responseString.startsWith("WELCOM ")){
                    int idGame=response[6];

                    byte[] arrb=new byte[2];
                    System.arraycopy(response, 8, arrb, 0, 2);
                    int heightMap=ByteBuffer.wrap(arrb).getInt();


                    System.arraycopy(response, 11, arrb, 0, 2);
                    int widthMap=ByteBuffer.wrap(arrb).getInt();

                    int nbrGhost=response[14];

                    byte[] ipb=new byte[15];
                    System.arraycopy(response, 16, ipb, 0, 15);
                    String ipMultiDiff=new String(ipb, StandardCharsets.UTF_8);

                    byte[] portb=new byte[4];
                    System.arraycopy(response, 32, portb, 0, 4);
                    int portMultiDiff=Integer.parseInt(new String(portb, StandardCharsets.UTF_8));

                    System.out.println("La partie "+idGame+" a pour hauteur "+heightMap+", largeur "+widthMap+", nombre de phantomes "+nbrGhost+", adresse ip de multidiffusion "+ipMultiDiff+" et port de multi diffusion"+portMultiDiff);
                    return true;
                }
                //TODO: changer position?
                //[POSIT id x y***]
                if(responseString.startsWith("POSIT ")){

                    byte[] arrIdb=new byte[8];
                    System.arraycopy(response, 6, arrIdb, 0, 8);
                    String idPlayer=new String(arrIdb, StandardCharsets.UTF_8);

                    byte[] corrdb=new byte[3];
                    System.arraycopy(response, 15, arrIdb, 0, 3);
                    int corrX=Integer.parseInt(new String(corrdb, StandardCharsets.UTF_8));

                    System.arraycopy(response, 19, arrIdb, 0, 3);
                    int corrY=Integer.parseInt(new String(corrdb, StandardCharsets.UTF_8));

                    System.out.println("Joueur: "+idPlayer+" a pour position x: "+corrX+" et pour position y:"+corrY);
                    return true;
                }else System.out.println(Error.requestClient);
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
            return false;
        }

        //[UNREG***]
        private boolean unReadyPlay(){
            try{
                //requestre
                String s="UNREG"+END_TCP;
                byte[] request=s.getBytes();
                this.ot.write(request);
                this.ot.flush();
                //response
                int idGame;
                byte[] response=readFirstMessage(in);

                String responseString=new String(response, StandardCharsets.UTF_8);
                if (responseString.startsWith("UNROK ")){
                    idGame=response[6];
                    System.out.println("Vous avez bien été désincrit de la partie: "+idGame);
                    return true;
                }else System.out.println(Error.requestClient);
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
            return false;
        }

        //[IQUIT***]
        private boolean quitGame(){
            try{
                String s="IQUIT"+END_TCP;
                byte[] request=s.getBytes();
                this.ot.write(request);
                this.ot.flush();
                byte[] response=readFirstMessage(this.in);
                String responseServeur=new String(response, StandardCharsets.UTF_8);
                if (responseServeur.equals("GOBYE"+END_TCP)){
                    System.out.println("Vous avez bien abandonné la partie.");
                    return true;
                }else System.out.println(Error.requestClient);
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
            return false;
        }

        //TODO: que faire si wait un long moment + gerer cas erreur default

        //[MAIL? mess***]
        private boolean messAllPlayer(String message){
            try{
                //request
                String s="MAIL? "+message+" "+END_TCP;
                byte[] request=s.getBytes();
                this.ot.write(request);
                this.ot.flush();
                //response
                byte[] response=readFirstMessage(this.in);
                String responseString= new String(response, StandardCharsets.UTF_8);
                if (responseString.equals("MAIL!"+END_TCP)) System.out.print("Le message a bien été envoyé");
                else if(responseString.equals("GOBYE"+END_TCP)){
                    System.out.println("La partie est terminé");
                    return true;
                }else System.out.println(Error.requestClient);
            }catch (IOException e){
                System.out.println(Error.responseServ);
            }
            return false;
        }

        //[SEND? id mess***]
        private boolean messOnePlayer(String message, String idPlayerDest){
            try{
                String s="SEND? "+idPlayerDest+" "+message+" "+END_TCP;
                byte[] request=s.getBytes();
                this.ot.write(request);
                this.ot.flush();
                byte[] response=readFirstMessage(this.in);
                String responseString= new String(response, StandardCharsets.UTF_8);
                switch (responseString){
                    case "SEND!"+END_TCP:{
                        System.out.print("Le message a bien été envoyé");
                        break;
                    }
                    case "NSEND!"+END_TCP:{
                        System.out.print("Le message n'a pas pu être envoyé");
                        break;
                    }
                    case "GOBYE"+END_TCP:{
                        System.out.println("La partie est terminé");
                        return true;
                    }
                    default:{
                        System.out.println(Error.requestClient);
                    }
                }
            }catch (IOException e){
                System.out.println(Error.responseServ);
            }
            return false;
        }

        //Deplacement

        //[DOMOV d***] [UPMOV d***] [RIMOV d***] [LEMOV d***]
        public boolean goMove(String move, String nbrCase){
            try{
                //request
                requestServMov(move, nbrCase);
                //response
                return responseServMov();
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.nbrArgs);
            }
            return false;
        }

        private void requestServMov(String direction, String nbrCase){
            try {
                String s=direction+" "+nbrCase+END_TCP;
                byte[] request=s.getBytes();
                MessagePlayer.this.ot.write(request);
                MessagePlayer.this.ot.flush();
            } catch (IOException e) {
                System.out.println(Error.requestServ);
            }
        }

        private boolean responseServMov(){
            byte[] response=readFirstMessage(MessagePlayer.this.in);
            String responseString=new String(response, StandardCharsets.UTF_8);
            if(responseString.startsWith("MOVE!") || responseString.startsWith("MOVEF")){

                byte[] coordb=new byte[3];
                System.arraycopy(response, 6, coordb, 0, 3);
                int coordX=Integer.parseInt(new String(coordb, StandardCharsets.UTF_8));

                System.arraycopy(response, 10, coordb, 0, 3);
                int coordY=Integer.parseInt(new String(coordb, StandardCharsets.UTF_8));

                byte[] nbrPts=new byte[4];
                System.arraycopy(response, 14, nbrPts, 0, 4);
                int nbrPtsPlayer=Integer.parseInt(new String(nbrPts, StandardCharsets.UTF_8));

                if(responseString.startsWith("MOVEF")){
                    System.out.println("Vos nouvelles coordonées sont: ("+coordX+","+coordY+") "+" votre nouveau score est de "+nbrPtsPlayer);
                }else System.out.println("Vos nouvelles coordonées sont: ("+coordX+","+coordY+")");
            }else if(responseString.startsWith("GOBYE")){
                System.out.println("La partie est terminé");
                return true;
            }else System.out.println(Error.requestClient);
            return false;
        }


    }

}