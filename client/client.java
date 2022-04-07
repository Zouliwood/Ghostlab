package client;

import java.net.DatagramSocket;

public class client{
    public static void main(String[] args) {
        try {
            DatagramSocket socket_client = new DatagramSocket(5555);
            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}