package client;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

class MessagePlayer {

    private final DataOutputStream ot;
    private final DataInputStream in;
    private final String pseudoClient;
    private final String END_TCP = "***";

    public MessagePlayer(DataOutputStream ot, DataInputStream in, String pseudoClient) {
        this.ot = ot;
        this.in = in;
        this.pseudoClient = pseudoClient;
    }

    // [SIZE? m***]
    public void sizeMaze(DataOutputStream ot, DataInputStream in, int idMap) {
        try {
            // request
            inRangeUint8(idMap);
            String s = "SIZE? m" + END_TCP;
            byte[] request = s.getBytes();
            request[6] = (byte) idMap;
            ot.write(request);
            ot.flush();
            // response
            int idGame, hGame, wGame;
            byte[] response = readFirstMessage(in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            // [SIZE! m h w***]
            if (responseString.startsWith("SIZE! ")) {
                idGame = response[6];

                byte[] hGameb = new byte[2];
                System.arraycopy(response, 8, hGameb, 0, 2);
                hGame = ByteBuffer.wrap(converting(hGameb)).getChar();

                byte[] wGameb = new byte[2];
                System.arraycopy(response, 11, wGameb, 0, 2);
                wGame = ByteBuffer.wrap(converting(wGameb)).getChar();

                System.out.println("Le labyrinthe " + idGame + " est de hauter " + hGame + " et de largeur " + wGame+".");
            } else
                System.out.println(Error.requestClient);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IndexOutOfBoundsException e) {
            System.out.println(Error.nbrArgs);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
    }

    // [LIST? m***] this.in && Player.this.in
    public void listPlayer(DataOutputStream ot, DataInputStream in, int idMap) {
        try {
            // request
            inRangeUint8(idMap);
            String s = "LIST? m" + END_TCP;
            byte[] request = s.getBytes();
            request[6] = (byte) idMap;
            ot.write(request);
            ot.flush();
            // response
            int idGame, nbrPlayers;
            byte[] response = readFirstMessage(in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("LIST! ")) {
                idGame = response[6];
                nbrPlayers = response[8];
                System.out.println("La partie " + idGame + " contient " + nbrPlayers + ((nbrPlayers>1)?" joueurs.":" joueur."));
                byte[] idPlayerb = new byte[8];
                if (nbrPlayers > 0)
                    System.out.println("La liste de joueurs :");
                else
                    System.out.println("Aucun joueur n'est inscrit à la partie.");
                for (int i = 0; i < nbrPlayers; i++) {
                    byte[] secResponse = readMessage(in);
                    String secResponseString = new String(secResponse, StandardCharsets.UTF_8);
                    if (secResponseString.startsWith("PLAYR ")) {
                        System.arraycopy(secResponse, 6, idPlayerb, 0, 8);
                        System.out.println("Le joueur : " + new String(idPlayerb, StandardCharsets.UTF_8));
                    }
                }
            } else
                System.out.println(Error.requestClient);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IndexOutOfBoundsException e) {
            System.out.println(Error.nbrArgs);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
    }

    // [GAME?***]
    public void listGame(DataOutputStream ot, DataInputStream in) {
        try {
            // request
            String s = "GAME?" + END_TCP;
            byte[] request = s.getBytes();
            ot.write(request);
            ot.flush();
            // response
            int nbrGames;
            byte[] response = readFirstMessage(in);
            if (response == null)
                throw new NullPointerException();
            String responseSting = new String(response, StandardCharsets.UTF_8);
            if (responseSting.startsWith("GAMES ")) {
                nbrGames = response[6];
                System.out.println("Il y a "+nbrGames+" parties possédant des joueurs et n'ayant toujours pas commencé.");
                for (int i = 0; i < nbrGames; i++) {
                    int idGame, nbrPlayers;
                    byte[] secResponse = readMessage(in);
                    String secResponseSting = new String(secResponse, StandardCharsets.UTF_8);
                    if (secResponseSting.startsWith("OGAME ")) {
                        idGame = secResponse[6];
                        nbrPlayers = secResponse[8];
                        System.out.println("La partie " + idGame + " possède " + nbrPlayers + "joueurs.");
                    } else
                        System.out.println(Error.responseServ);
                }
            } else
                System.out.println(Error.responseServ);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.nbrArgs);
            e.printStackTrace();
        }
    }

    // ----------------------------------------------------------------------------------------------------------------------

    // [START***]
    public String[] readyPlay() {
        try {
            // request
            String s = "START" + END_TCP;
            byte[] request = s.getBytes();
            this.ot.write(request);
            this.ot.flush();

            byte[] response;
            String responseString;

            while (true) {
                // pour ne pas saturer le processeur
                try {
                    Thread.sleep(1);
                } catch (InterruptedException ignored) {
                }

                // response
                response = readFirstMessage(in);
                if (response != null) {
                    responseString = new String(response, StandardCharsets.UTF_8);
                    // [WELCO m h w f ip port***]
                    if (responseString.startsWith("WELCO ")) {
                        int idGame = response[6];

                        byte[] arrb = new byte[2];
                        System.arraycopy(response, 8, arrb, 0, 2);
                        int heightMap = ByteBuffer.wrap(converting(arrb)).getChar();

                        System.arraycopy(response, 11, arrb, 0, 2);
                        int widthMap = ByteBuffer.wrap(converting(arrb)).getChar();

                        int nbrGhost = response[14];

                        byte[] ipb = new byte[15];
                        System.arraycopy(response, 16, ipb, 0, 15);
                        String ipMultiDiff = (new String(ipb, StandardCharsets.UTF_8)).replace("#","");

                        byte[] portb = new byte[4];
                        System.arraycopy(response, 32, portb, 0, 4);
                        int portMultiDiff = Integer.parseInt(new String(portb, StandardCharsets.UTF_8));

                        System.out.println("La partie " + idGame + " a pour hauteur " + heightMap + ", largeur "
                                + widthMap + ", nombre de fantomes " + nbrGhost + ", adresse ip de multidiffusion "
                                + ipMultiDiff + " et port de multidiffusion " + portMultiDiff);
                        return new String[]{
                                ipMultiDiff, Integer.toString(portMultiDiff),
                                Integer.toString(widthMap), Integer.toString(heightMap)
                        };
                        //return new PlayerMulticast(ipMultiDiff, portMultiDiff);
                    }
                }
            }
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
        return null;
    }

    public String[] getposIT(){
        // response
        byte[] response = readFirstMessage(in);
        if (response == null)
            throw new NullPointerException();
        String responseString = new String(response, StandardCharsets.UTF_8);

        // [POSIT id x y***]
        if (responseString.startsWith("POSIT ")) {

            String idPlayer = responseString.substring(6, 14);
            String corrX = responseString.substring(15, 18);
            String corrY = responseString.substring(19, 22);

            System.out.println("Joueur : " + idPlayer + " a pour position (" + corrY + "," + corrX + ").");
            return new String[]{"true", corrY, corrX};
        } else
            System.out.println(Error.requestClient);
        return new String[]{"false"};
    }

    // [UNREG***]
    public boolean unReadyPlay() {
        try {
            // requestre
            String s = "UNREG" + END_TCP;
            byte[] request = s.getBytes();
            this.ot.write(request);
            this.ot.flush();
            // response
            int idGame;
            byte[] response = readFirstMessage(in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("UNROK ")) {
                idGame = response[6];
                System.out.println("Vous avez bien été désinscrit de la partie " + idGame+".");
                return true;
            } else
                System.out.println(Error.requestClient);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
        return false;
    }

    // [IQUIT***]
    public boolean quitGame() {
        try {
            String s = "IQUIT" + END_TCP;
            byte[] request = s.getBytes();
            this.ot.write(request);
            this.ot.flush();
            byte[] response = readFirstMessage(this.in);
            if (response == null)
                throw new NullPointerException();
            String responseServeur = new String(response, StandardCharsets.UTF_8);

            if (responseServeur.startsWith("GOBYE" + END_TCP)) {
                System.out.println("Vous avez bien abandonné la partie.");
                return true;
            } else System.out.println(Error.requestClient);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
        return false;
    }

    // [MALL? mess***]
    public boolean messAllPlayer(String message) {
        try {
            // request
            String s = "MALL? " + message + END_TCP;
            byte[] request = s.getBytes();
            this.ot.write(request);
            this.ot.flush();
            // response
            byte[] response = readFirstMessage(this.in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("MALL!" + END_TCP))
                System.out.println("Le message a bien été envoyé.");
            else if (responseString.startsWith("GOBYE" + END_TCP)) {
                System.out.println("La partie est terminée.");
                return true;
            } else
                System.out.println(Error.requestClient);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.responseServ);
        }
        return false;
    }

    // [SEND? id mess***]
    public boolean messOnePlayer(String message, String idPlayerDest) {
        try {
            String s = "SEND? " + idPlayerDest + " " + message + END_TCP;
            byte[] request = s.getBytes();
            this.ot.write(request);
            this.ot.flush();
            byte[] response = readFirstMessage(this.in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("SEND!" + END_TCP)){
                System.out.println("Le message a bien été envoyé.");
            }else if(responseString.startsWith("NSEND" + END_TCP)){
                System.out.println("Le message n'a pas pu être envoyé.");
            }else if(responseString.startsWith("GOBYE" + END_TCP)){
                System.out.println("La partie est terminée.");
            }else System.out.println(Error.requestClient);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.responseServ);
        }
        return false;
    }

    // Deplacement

    // [DOMOV d***] [UPMOV d***] [RIMOV d***] [LEMOV d***]
    public String[] goMove(int direction, String nbrCase) {
        String move;
        if (direction==0) move="UPMOV";
        else if (direction==1) move="RIMOV";
        else if (direction==2) move="DOMOV";
        else move="LEMOV";
        try {
            // request
            requestServMov(move, nbrCase);
            // response
            byte[] response = readFirstMessage(MessagePlayer.this.in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("MOVE!")) {
                String coordX = responseString.substring(6,9);
                String coordY = responseString.substring(10, 13);
                System.out.println("Vos nouvelles coordonnées sont (" + coordY + "," + coordX + ").");
                return new String[]{"false", coordY, coordX};
            } else if(responseString.startsWith("MOVEF")){
                String coordX = responseString.substring(6,9);
                String coordY = responseString.substring(10, 13);
                String nbrPtsPlayer = responseString.substring(14, 18);
                System.out.println("Vos nouvelles coordonnées sont (" + coordY + "," + coordX + ") "
                        + " et votre nouveau score est de " + nbrPtsPlayer+".");
                return new String[]{"false", coordY, coordX};
            } else if (responseString.startsWith("GOBYE")) {
                System.out.println("La partie est terminée.");
                return new String[]{"true"};
            } else System.out.println(Error.requestClient);
            return new String[]{"false"};
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        }
        return new String[]{"false"};
    }

    public void initPlayer() throws IOException {
        // [GAME n***]
        int nbrPlayersWaiting;
        if (readInput(in, 6).equals("GAMES ")) {
            nbrPlayersWaiting = in.readUnsignedByte() & 0xFF;
            System.out.println("Il y a "+nbrPlayersWaiting+((nbrPlayersWaiting>1)?" parties en attente.":" partie en attente."));
            if (!readInput(in, 3).equals(END_TCP)) {
                System.out.println(Error.responseServ);
                throw new IOException();
            }
        } else {
            System.out.println(Error.responseServ);
            throw new IOException();
        }

        // [OGAME m s***]
        for (int i = 0; i < nbrPlayersWaiting; i++) {
            if (readInput(in, 6).equals("OGAME ")) {
                int nbrGame = in.readUnsignedByte() & 0xFF;
                readInput(in, 1);
                int nbrPlayersRegistred = in.readUnsignedByte() & 0xFF;
                System.out.println("La partie "+nbrGame+" a "+nbrPlayersRegistred
                        +((nbrPlayersRegistred>1)?" joueurs dans sa salle d'attente.":" joueur dans sa salle d'attente."));
                if (!readInput(in, 3).equals(END_TCP)) {
                    System.out.println(Error.responseServ);
                    throw new IOException();
                }
            } else {
                System.out.println(Error.responseServ);
                throw new IOException();
            }
        }
    }

    // [NEWPL id port***]
    public boolean createGame(String UDPPlayer) {
        try {
            // request
            String s = "NEWPL " + this.pseudoClient + " port" + END_TCP;
            byte[] request = s.getBytes();
            byte[] udpplayer = UDPPlayer.getBytes();
            System.arraycopy(udpplayer, 0, request, 15, 4);
            this.ot.write(request);
            this.ot.flush();
            // response
            int idGame;
            byte[] response = readFirstMessage(this.in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("REGOK")) {
                idGame = response[6];
                System.out.println("Vous êtes maintenant inscrit à la partie " + idGame + ".");
                return true;
            } else if (responseString.startsWith("REGNO"))
                System.out.println("La partie n'a pas pu être créée.");
            else
                System.out.println(Error.responseServ);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IndexOutOfBoundsException e) {
            System.out.println(Error.valueInt);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
        return false;
    }

    // [REGIS id port m***]
    public boolean joinGame(String UDPPlayer, int idGame) {
        try {
            // request
            String s = "REGIS " + this.pseudoClient + " port m" + END_TCP;
            byte[] request = s.getBytes();
            inRangeUint8(idGame);
            request[20] = (byte) idGame;
            byte[] udpplayer = UDPPlayer.getBytes();
            System.arraycopy(udpplayer, 0, request, 15, 4);
            this.ot.write(request);
            this.ot.flush();
            // response
            int idGameJ;
            byte[] response = readFirstMessage(this.in);
            if (response == null)
                throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("REGOK")) {
                idGameJ = response[6];
                System.out.println("Vous êtes maintenant inscrit à la partie " + idGameJ+".");
                return true;
            } else if (responseString.startsWith("REGNO"))
                System.out.println("Vous n'avez pas pu rejoindre la partie.");
            else
                System.out.println(Error.responseServ);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IndexOutOfBoundsException e) {
            System.out.println(Error.valueInt);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
        return false;
    }

    public boolean listPlayerInGame(DataOutputStream ot, DataInputStream in) {
        try {
            String messageString = "GLIS?" + END_TCP;
            byte[] request = messageString.getBytes();
            ot.write(request);
            ot.flush();

            // response
            int nbrJoueur;
            byte[] response = readFirstMessage(in);
            if (response == null) throw new NullPointerException();
            String responseString = new String(response, StandardCharsets.UTF_8);
            // [SIZE! m h w***]
            if (responseString.startsWith("GLIS! ")) {
                nbrJoueur = response[6];
                for (int i = 0; i < nbrJoueur; i++) {
                    response = readMessage(in);
                    responseString = new String(response, StandardCharsets.UTF_8);

                    if (responseString.startsWith("GPLYR ")) {

                        String idPlayer = responseString.substring(6, 14);
                        String corrX = responseString.substring(15, 18);
                        String corrY = responseString.substring(19, 22);
                        String pts = responseString.substring(23, 27);

                        System.out.println("Le joueur " + idPlayer + " est en position (" + corrY + "," + corrX + ") et a "
                                + pts + " points.");
                    } else
                        System.out.println(Error.responseServ);
                }
            } else if (responseString.startsWith("GOBYE" + END_TCP)) {
                System.out.println("La partie est terminée.");
                return true;
            } else
                System.out.println(Error.requestClient);
        } catch (NullPointerException e) {
            System.out.println(Error.responseServ);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
        }
        return false;
    }

    // -------------------------Aux Function----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------

    private void inRangeUint8(int value) throws IndexOutOfBoundsException {
        if (value > 255 || value < 0) {
            throw new IndexOutOfBoundsException();
        }
    }

    private byte[] readFirstMessage(DataInputStream data) {
        byte[] response = new byte[50];
        try {
            int cpt = 0;
            int nbrStars = 0;
            while (nbrStars != 3) {
                if (cpt == 50)
                    return null;
                response[cpt] = (byte) data.read();
                if (response[cpt] == 42)
                    nbrStars++;
                else
                    nbrStars = 0;
                cpt++;
            }
        } catch (IOException ignored) {
        }
        return response;
    }

    private byte[] readMessage(DataInputStream data) {
        return readFirstMessage(data);
    }

    private byte[] converting(byte[] value) {
        final int length = value.length;
        byte[] res = new byte[length];
        for (int i = 0; i < length; i++) {
            res[length - i - 1] = value[i];
        }
        return res;
    }

    private void requestServMov(String direction, String nbrCase) {
        try {
            String s = direction + " " + nbrCase + END_TCP;
            byte[] request = s.getBytes();
            MessagePlayer.this.ot.write(request);
            MessagePlayer.this.ot.flush();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
        }
    }

    private String readInput(DataInputStream in, int size) throws IOException {
        // request
        byte[] input_header = new byte[size];
        int result = in.read(input_header, 0, size);
        if (result == -1) {
            System.out.println(Error.responseServ);
            throw new IOException();
        }
        // response
        return new String(input_header, StandardCharsets.UTF_8);
    }
}