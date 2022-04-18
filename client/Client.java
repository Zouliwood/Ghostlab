package client;

public class Client {

    public static void main(String[] args) {
        if (args.length!=2){
            System.out.println(Error.nbrArgs);
            return;
        }
        try {
            (new User.ConnectServerTCP(args[0], Integer.parseInt(args[1]))).run();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
