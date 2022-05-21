package client;

public class Plateau {
    private final int [][] plateau;//0 non decouvert + 1 praticable + 2 mur
    private final Joueur player;
    private static final String ANSI_RESET = "\u001B[0m";
    private static final String BLUE = "\033[0;34m";

    Plateau(int height, int width, int initX, int initY){
        plateau=new int[height][width];
        for (int i = 0; i < plateau.length; i++) {
            for (int j = 0; j < plateau[0].length; j++) {
                plateau[i][j]=0;
            }
        }
        this.player= new Joueur(initX, initY);
    }

   void addWall(int direction, int nbrCase, int newX, int newY){
        player.updatePos(newX, newY);
        if (player.isBlocked(direction, nbrCase)){
            if (direction==0){//haut
                if (player.y-1>=0)this.plateau[player.y-1][player.x]=2;
            } else if (direction==1) {//droite
                if (player.x+1<plateau[0].length)this.plateau[player.y][player.x+1]=2;
            } else if (direction==2) {//bas
                if (player.y+1<plateau.length)this.plateau[player.y+1][player.x]=2;
            }else{//gauche
                if (player.x-1>=0)this.plateau[player.y][player.x-1]=2;
            }
        }
   }

    void printPlateau(){
        for (int i = 0; i<plateau.length; i++) {
            for (int j = 0; j < plateau[0].length; j++) {
                if (player.y==i && player.x==j){
                    System.out.print(BLUE+"\u2B1B "+ANSI_RESET);
                }else System.out.print((plateau[i][j]==2)?"\u2B1B ":"\u2B1C ");
            }
            System.out.println();
        }
    }

    private static class Joueur{
        private int oldx, oldy;
        private int x, y;//val courrante de retour serv

        Joueur(int x, int y){
            this.oldx=x;
            this.oldy=y;
            this.x=x;
            this.y=y;
        }

        boolean isBlocked(int direction, int nbrCase){
            if (direction==0){//haut
                return oldy-nbrCase<y;
            }else if (direction==1){//droite
                return oldx+nbrCase>x;
            }else if (direction==2){//bas
                return oldy+nbrCase>y;
            }else{//gauche
                return oldx-nbrCase<x;
            }
        }
        void updatePos(int x, int y){
            this.oldx=this.x;
            this.oldy=this.y;
            this.x=x;
            this.y=y;
        }
    }
}
