package client;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramSocket;
import java.net.SocketException;

public class Player {

    //TODO: do enum ?
    private static final int SIZE_OF_HEAD=5;
    private static final int SIZE_OF_END=3;
    private static final String END_TCP="***";

    private final int idPlayer;
    private final DataOutputStream ot;

    public Player(int idPlayer, DataOutputStream ot){
        this.idPlayer=idPlayer;
        this.ot=ot;
    }

    class ConnectServerUDP implements Runnable {

        private final String hostName;
        private final int port;

        public ConnectServerUDP(String hostName, int port){
            this.port=port;
            this.hostName=hostName;
        }

        @Override
        public void run() {
            try {
                DatagramSocket socket_client = new DatagramSocket(port);
                //TODO: udp connection
            } catch (SocketException e) {
                e.printStackTrace();
            }
        }
    }

    private class Message implements MessageUserPlayer{

        //[START***]
        private void readyPlay(){
            try{
                String s="START"+END_TCP;
                byte[] request=s.getBytes();
                Player.this.ot.write(request);
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

        //[UNREG***]
        private void unReadyPlay(){
            try{
                String s="UNREG"+END_TCP;
                byte[] request=s.getBytes();
                Player.this.ot.write(request);
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

        //[IQUIT***]
        private void quitGame(){
            try{
                String s="IQUIT"+END_TCP;
                byte[] request=s.getBytes();
                Player.this.ot.write(request);
            } catch (IOException e) {
                System.out.println(Error.requestServ);
                e.printStackTrace();
            }
        }

        //TODO: UDP?
        //[MAIL? mess***]
        private void messAllPlayer(){

        }

        //TODO: UDP?
        //[SEND? id mess***]
        private void messOnePlayer(){

        }

        @Override
        public void sizeMaze(int idMap) {
            try{
                inRangeUint8(idMap);
                String s="SIZE? m"+END_TCP;
                byte[] request=s.getBytes();
                request[6]=(byte)idMap;
                Player.this.ot.write(request);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.nbrArgs);
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
                Player.this.ot.write(request);
            }catch(IndexOutOfBoundsException e){
                System.out.println(Error.nbrArgs);
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
                Player.this.ot.write(request);
            } catch (IOException e) {
                System.out.println(Error.nbrArgs);
                e.printStackTrace();
            }
        }

        private class Deplacement{

            //[LEMOV d***]
            private void goLeft(int nbrCase){
                try{
                    inRangeUint8(nbrCase);
                    String s="LEMOV d"+END_TCP;
                    byte[] request=s.getBytes();
                    request[6]=(byte)nbrCase;
                    Player.this.ot.write(request);
                }catch(IndexOutOfBoundsException e){
                    System.out.println(Error.nbrArgs);
                    e.printStackTrace();
                } catch (IOException e) {
                    System.out.println(Error.requestServ);
                    e.printStackTrace();
                }
            }

            //[RIMOV d***]
            private void goRight(int nbrCase){
                try{
                    inRangeUint8(nbrCase);
                    String s="RIMOV d"+END_TCP;
                    byte[] request=s.getBytes();
                    request[6]=(byte)nbrCase;
                    Player.this.ot.write(request);
                }catch(IndexOutOfBoundsException e){
                    System.out.println(Error.nbrArgs);
                    e.printStackTrace();
                } catch (IOException e) {
                    System.out.println(Error.requestServ);
                    e.printStackTrace();
                }
            }

            //[UPMOV d***]
            private void goTop(int nbrCase){
                try{
                    inRangeUint8(nbrCase);
                    String s="UPMOV d"+END_TCP;
                    byte[] request=s.getBytes();
                    request[6]=(byte)nbrCase;
                    Player.this.ot.write(request);
                }catch(IndexOutOfBoundsException e){
                    System.out.println(Error.nbrArgs);
                    e.printStackTrace();
                } catch (IOException e) {
                    System.out.println(Error.requestServ);
                    e.printStackTrace();
                }
            }

            //[DOMOV d***]
            private void goBottom(int nbrCase){
                try{
                    inRangeUint8(nbrCase);
                    String s="DOMOV d"+END_TCP;
                    byte[] request=s.getBytes();
                    request[6]=(byte)nbrCase;
                    Player.this.ot.write(request);
                }catch(IndexOutOfBoundsException e){
                    System.out.println(Error.nbrArgs);
                    e.printStackTrace();
                } catch (IOException e) {
                    System.out.println(Error.requestServ);
                    e.printStackTrace();
                }
            }

        }
    }

}