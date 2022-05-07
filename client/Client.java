package client;

public class Client {

    public static void main(String[] args) {
        if (args.length == 2) {
            try {
                (new PlayerTCP(args[0], Integer.parseInt(args[1]))).run();
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else if (args.length == 3) {
            try {
                (new PlayerTCP(args[0], Integer.parseInt(args[1]), args[2])).run();
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else System.out.println(Error.nbrArgs);
    }

}