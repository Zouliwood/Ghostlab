package client;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;

public class PlayerMulticast implements Runnable{

    private final String hostName;
    private final int port;
    private MulticastSocket mso;

    public PlayerMulticast(String hostName, int port) {
        this.port = port;
        this.hostName = hostName;
        mso=null;
    }

    @Override
    public void run() {
        try{
            mso=new MulticastSocket(port);
            mso.joinGroup(InetAddress.getByName(hostName));
            byte[]data=new byte[65535];//max size udp/multicast
            DatagramPacket paquet=new DatagramPacket(data,data.length);
            while(true){
                mso.receive(paquet);
                String responseString=new String(paquet.getData(),0,paquet.getLength());
                String header=responseString.substring(0, 5);
                switch (header){
                    case "GHOST":{//[GHOST x y +++]
                        String x_pos=responseString.substring(6, 9);
                        String y_pos=responseString.substring(10, 13);
                        System.out.print("Un fantome est en position: ("+x_pos+","+y_pos+")");
                        break;
                    }
                    case "SCORE":{//[SCORE id p x y+++]
                        String id_player=responseString.substring(6, 14);
                        String pts_player=responseString.substring(15, 19);
                        String x_pos=responseString.substring(20, 23);
                        String y_pos=responseString.substring(24, 27);
                        System.out.println("Le joueur "+id_player+" a "+pts_player+" points et est en position ("+x_pos+","+y_pos+")");
                        break;
                    }
                    case "MESSA":{//MESSA id mess+++]
                        String id_player=responseString.substring(6, 14);
                        String mess=responseString.substring(15, responseString.length()-3);
                        System.out.println("Le joueur "+id_player+" a envoyé le message: "+mess);
                        break;
                    }
                    case "ENDGA":{//[ENDGA id p+++]
                        String id_player=responseString.substring(6, 14);
                        String pts_player=responseString.substring(15, 19);
                        System.out.println("Le joueur "+id_player+" a "+pts_player);
                        break;
                    }
                }
            }
        } catch(Exception e){
            System.out.println("Socket Multicast close");
        }
    }

    public void close(){
        mso.close();
    }

}