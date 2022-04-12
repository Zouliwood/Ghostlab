package client;

import java.io.*;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

public class User {

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
                    System.out.format("Il y a %d joueurs en salle d'attente.\n",nbrPlayersWaiting);
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
                while (pseudoClient.length()!=8){
                    System.out.println("Veuillez saisir votre pseudo (8 charactères alphanumériques):");
                    pseudoClient=new Scanner(System.in).nextLine();
                }

                int clientUDP=0;
                while (clientUDP<1000 || clientUDP>9999){
                    System.out.println("Veuillez saisir votre pseudo (4 chiffres):");
                    clientUDP = new Scanner(System.in).nextInt();
                }

                Message m=new Message(ot, in, pseudoClient, clientUDP);
                Player p=new Player(ot, in, pseudoClient, clientUDP);

                int gameChoice=-1;
                while (gameChoice!=0 && gameChoice!=1){
                    System.out.println("Voulez créer [0] ou rejoindre [1] une partie:");
                    gameChoice=new Scanner(System.in).nextInt();
                }
                if (gameChoice == 0) {
                    m.createGame(pseudoClient, clientUDP);
                } else {
                    int idGame = -1;
                    while (idGame < 0 || idGame > 255) {
                        System.out.println("Entrez le numéro de la partie que vous souhaitez rejoindre:");
                        idGame = new Scanner(System.in).nextInt();
                    }
                    m.joinGame(clientUDP, idGame);
                }
                while (true){
                    //TODO: scanner for player
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
        public void createGame(String idPlayer, int UDPPlayer) {
            try{
                String s="NEWPL "+this.pseudoClient+" port"+END_TCP;
                byte[] request=s.getBytes();
                byte[]udpplayer=ByteBuffer.allocate(4).putInt(UDPPlayer).array();
                System.arraycopy(udpplayer, 0, request, 15, 4);
                this.ot.write(request);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.valueInt);
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

        //[REGIS id port m***]
        public void joinGame(int UDPPlayer, int idGame){
            try{
                String s="REGIS "+this.pseudoClient+" port m"+END_TCP;
                byte[] request=s.getBytes();
                inRangeUint8(idGame);
                request[20]=(byte)idGame;
                byte[]udpplayer=ByteBuffer.allocate(4).putInt(UDPPlayer).array();
                System.arraycopy(udpplayer, 0, request, 15, 4);
                this.ot.write(request);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.valueInt);
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

        @Override
        public void sizeMaze(int idMap) {
            try{
                inRangeUint8(idMap);
                String s="SIZE? m"+END_TCP;
                byte[] request=s.getBytes();
                request[6]=(byte)idMap;
                this.ot.write(request);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.valueInt);
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

        @Override
        public void listPlayer(int idMap) {
            try{
                inRangeUint8(idMap);
                String s="LIST? m"+END_TCP;
                byte[] request=s.getBytes();
                request[6]=(byte)idMap;
                this.ot.write(request);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.valueInt);
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

        @Override
        public void listGame() {
            try{
                String s="GAME?"+END_TCP;
                byte[] request=s.getBytes();
                this.ot.write(request);
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

    }

}