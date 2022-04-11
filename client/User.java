package client;

import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class User {

    //TODO: do enum ?
    private static final int SIZE_OF_HEAD=5;
    private static final int SIZE_OF_END=3;
    private static final String END_TCP="***";

    static class ConnectServerTCP implements Runnable {

        //TODO: do enum?
        private static final String GAMES="GAMES ";
        private static final String OGAME="OGAME ";

        private final String hostName;
        private final int port;
        //SendMessage.Deplacement movement;

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
                if (readInput(in, SIZE_OF_HEAD+1).equals(GAMES)){
                    nbrPlayersWaiting=in.readUnsignedByte() & 0xFF;
                    System.out.format("Il y a %d joueurs en salle d'attente.\n",nbrPlayersWaiting);
                    if (!readInput(in, SIZE_OF_END).equals(END_TCP)){
                        System.out.println(Error.responseServ);
                        throw new IOException();
                    }
                }else{
                    System.out.println(Error.responseServ);
                    throw new IOException();
                }

                //[OGAME m s***]
                for (int i = 0; i < nbrPlayersWaiting; i++) {
                    if (readInput(in, SIZE_OF_HEAD+1).equals(OGAME)){
                        int nbrGame=in.readUnsignedByte() & 0xFF;
                        readInput(in,1);
                        int nbrPlayersRegistred=in.readUnsignedByte() & 0xFF;
                        System.out.format("La partie numéro %d a %d joueurs dans sa salle d'attente.\n", nbrGame, nbrPlayersRegistred);
                        if (!readInput(in, SIZE_OF_END).equals(END_TCP)){
                            System.out.println(Error.responseServ);
                            throw new IOException();
                        }
                    }else{
                        System.out.println(Error.responseServ);
                        throw new IOException();
                    }
                }

                //TODO: SendMessage messPlayer=new SendMessage(ot, ID, UDP);
                //TODO: scanner for player

                //SendMessage messPlayer=new SendMessage(ot, 1, 1);
                //InGame.Deplacement.goLeft(10);

                Message m = new Message(ot);
                m.listGame();

            } catch (Exception e) {
                System.out.println(e);
                e.printStackTrace();
            }
        }
    }

    private static class Message implements MessageUserPlayer{

        DataOutputStream ot;

        public Message(DataOutputStream ot){
            this.ot=ot;
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