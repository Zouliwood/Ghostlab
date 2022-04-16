package client;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Player {

    private final String idPlayer;
    private final DataOutputStream ot;
    private final DataInputStream in;

    public Player(DataOutputStream ot, DataInputStream in, String idPlayer, int UDPClient){
        this.idPlayer=idPlayer;
        this.ot=ot;
        this.in=in;
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
            } catch (SocketException e) {
                e.printStackTrace();
            }
        }
    }

}