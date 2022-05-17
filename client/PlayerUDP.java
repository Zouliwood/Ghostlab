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

            //TODO: close udp?
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}