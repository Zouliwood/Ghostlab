package client;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.DatagramSocket;
import java.net.SocketException;

public class Player {

    public Player(int UDPClient){
        (new ConnectServerUDP("", UDPClient)).run();
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
                //TODO: multi-diffusion
            } catch (SocketException e) {
                e.printStackTrace();
            }
        }
    }

}