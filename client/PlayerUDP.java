package client;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Socket;

public class PlayerUDP implements Runnable {

    private final int port;
    private DatagramSocket dso;

    public PlayerUDP(int port) {
        this.port = port;
        dso=null;
    }

    @Override
    public void run() {
        try {
            dso=new DatagramSocket(port);
            while(true){
                //TODO: debug
                byte[]data=new byte[65535];//taille max possible avec DatagramPacket
                System.out.println("okkay1");
                DatagramPacket paquet=new DatagramPacket(data,data.length);
                System.out.println("okkay2");
                dso.receive(paquet);
                System.out.println("okkay3");
                String st=new String(paquet.getData(),0,paquet.getLength());
                if (st.startsWith("MESSP ")){
                    String idSender=st.substring(6, 14);
                    String messSender=st.substring(15);
                    System.out.println("Message du joueur "+idSender+": "+messSender);
                }else System.out.println(Error.responseServ);
            }

        } catch (IOException e) {
            System.out.println("Socket UDP close");
        }
    }

    public void close(){
        dso.close();
    }

}