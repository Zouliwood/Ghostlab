package client;

import java.io.Serializable;

public class Historique implements Serializable {
    private String pseudo;
    private long totalTimeInGame = 0;

    public String getPseudo() {
        return pseudo;
    }

    public void setPseudo(String pseudo) {
        this.pseudo = pseudo;
    }

    public void newGame() {
        Timer.start();
    }

    public void endGame() {
        Timer.stop();
        System.out.println(Timer.timeToString());
        this.totalTimeInGame += Timer.getDuree();
        System.out.println("Temps de jeu total : " + this.totalTimeInGame);
    }

    private static class Timer {

        private static long tempsDepart = 0;
        private static long tempsFin = 0;
        private static long duree = 0;

        public static void start() {
            tempsDepart = System.currentTimeMillis();
            tempsFin = 0;
            duree = 0;
        }

        public static void stop() {
            if (tempsDepart == 0) {
                return;
            }
            tempsFin = System.currentTimeMillis();
            duree = (tempsFin - tempsDepart);
            tempsDepart = 0;
            tempsFin = 0;
        }

        public static long getDuree() {
            return duree;
        }

        public static String timeToString() {

            int hours = (int) ((duree / 1000) / 3600);
            int minutes = (int) (((duree / 1000) % 3600) / 60);
            int secondes = (int) ((duree / 1000) % 60);

            String response = "Durée de la partie : ";

            if (hours > 0) {
                response += hours + " h ";
            }
            if (minutes > 0) {
                response += minutes + " min ";
            }
            if (secondes > 0) {
                response += secondes + " s";
            }
            if (hours <= 0 && minutes <= 0 && secondes <= 0) {
                response = "0 s";
            }

            return response;
        }

    }

}