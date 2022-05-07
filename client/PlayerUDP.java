package client;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

public class PlayerUDP implements Runnable {
    private final int port;

    public PlayerUDP(int port) {
        this.port = port;
    }

    @Override
    public void run() {
        try {
            DatagramSocket dso=new DatagramSocket(port);
            while(true){
                //
                byte[]data=new byte[65535];//taille max possible avec DatagramPacket
                DatagramPacket paquet=new DatagramPacket(data,data.length);
                dso.receive(paquet);
                String st=new String(paquet.getData(),0,paquet.getLength());
                if (st.startsWith("MESSP ")){
                    String idSender=st.substring(6, 14);
                    String messSender=st.substring(15);
                    System.out.println("Message du joueur "+idSender+": "+messSender);
                }else System.out.println(Error.responseServ);
            }

            //TODO: close udp?
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}