package client;

import java.io.IOException;
import java.net.*;

public class PlayerUDP implements Runnable {

    private final DatagramSocket dso;
    public static final String ANSI_RESET = "\u001B[0m";
    public static final String ANSI_GREEN = "\u001B[32m";

    public PlayerUDP(int port) throws SocketException {
        dso=new DatagramSocket(port);
    }

    @Override
    public void run() {
        try {
            while(true){
                byte[]data=new byte[65535];//taille max possible avec DatagramPacket
                DatagramPacket paquet=new DatagramPacket(data,data.length);
                dso.receive(paquet);
                String st=new String(paquet.getData(),0,paquet.getLength());
                if (st.startsWith("MESSP ")){
                    String idSender=st.substring(6, 14);
                    String messSender=st.substring(15, st.length()-3);
                    System.out.println(ANSI_GREEN+"Message du joueur "+idSender+" : "+messSender+ANSI_RESET);
                }else System.out.println(ANSI_GREEN+Error.responseServ+ANSI_RESET);
            }
        }catch (IOException e) {
            System.out.println(ANSI_GREEN+"Fin UDP."+ANSI_RESET);
        }
    }

    public void close(){
        dso.close();
    }

}