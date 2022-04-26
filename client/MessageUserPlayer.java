package client;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public interface MessageUserPlayer {

    String END_TCP="***";

    default void inRangeUint8(int value) throws IndexOutOfBoundsException{
        if (value>255 || value<0){
            throw new IndexOutOfBoundsException();
        }
    }

    default byte[] readFirstMessage(DataInputStream data){
        byte[] response=new byte[50];
        try{
            int cpt=0;
            int nbrStars=0;
            while (nbrStars!=3) {
                if (cpt==50)return null;
                response[cpt]=(byte)data.read();
                if (response[cpt]==42) nbrStars++;
                else nbrStars=0;
                cpt++;
            }
        }catch (IOException ignored){}
        return response;
    }

    default byte[] readMessage(DataInputStream data) {
        return readFirstMessage(data);
    }

    //TODO: vider le in en cas d'erreur?

    //[SIZE? m***]
    default void sizeMaze(DataOutputStream ot, DataInputStream in, int idMap) {
        try{
            //request
            inRangeUint8(idMap);
            String s="SIZE? m"+END_TCP;
            byte[] request=s.getBytes();
            request[6]=(byte)idMap;
            ot.write(request);
            ot.flush();
            //response
            int idGame, hGame, wGame;
            byte[] response=readFirstMessage(in);
            String responseString= new String(response, StandardCharsets.UTF_8);
            //[SIZE! m h w***]
            if (responseString.startsWith("SIZE! ")){
                idGame=response[6];

                byte[] hGameb=new byte[2];
                System.arraycopy(response, 8, hGameb, 0, 2);
                hGame = ByteBuffer.wrap(converting(hGameb)).getChar();

                byte[] wGameb=new byte[2];
                System.arraycopy(response, 11, wGameb, 0, 2);
                wGame =  ByteBuffer.wrap(converting(wGameb)).getChar();

                System.out.println("La map "+idGame+" est de hauter "+hGame+" et de largeur "+wGame);
            }else System.out.println(Error.requestClient);
        }catch(IndexOutOfBoundsException e){
            System.out.println(Error.nbrArgs);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
    }

    static byte[] converting(byte[] value) {
        final int length = value.length;
        byte[] res = new byte[length];
        for(int i = 0; i < length; i++) {
            res[length - i - 1] = value[i];
        }
        return res;
    }


    //[LIST? m***] this.in && Player.this.in
    default boolean listPlayer(DataOutputStream ot, DataInputStream in, int idMap) {
        try{
            //request
            inRangeUint8(idMap);
            String s="LIST? m"+END_TCP;
            byte[] request=s.getBytes();
            request[6]=(byte)idMap;
            ot.write(request);
            ot.flush();
            //response
            int idGame, nbrPlayers;
            byte[] response=readFirstMessage(in);
            String responseString=new String(response, StandardCharsets.UTF_8);
            if (responseString.startsWith("LIST! ")){
                idGame=response[6];
                nbrPlayers=response[8];
                System.out.println("La partie "+idGame+" contient "+nbrPlayers+" joueurs.");
                byte[] idPlayerb=new byte[8];
                if (nbrPlayers>0)System.out.println("La liste des joueurs:");
                else System.out.println("Aucun joueur n'est inscrit dans la parite");
                for (int i = 0; i < nbrPlayers; i++) {
                    byte[] secResponse=readMessage(in);
                    String secResponseString=new String(secResponse, StandardCharsets.UTF_8);
                    if (secResponseString.startsWith("PLAYR ")){
                        System.arraycopy(secResponse, 6, idPlayerb, 0, 8);
                        System.out.println("Le joueur: "+new String(idPlayerb, StandardCharsets.UTF_8));
                    }
                }
            }else if(responseString.equals("GOBYE"+END_TCP)){
                System.out.println("La partie est terminé");
                return true;
            }else System.out.println(Error.requestClient);
        }catch(IndexOutOfBoundsException e){
            System.out.println(Error.nbrArgs);
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println(Error.requestServ);
            e.printStackTrace();
        }
        return false;
    }

    //[GAME?***]
    default void listGame(DataOutputStream ot, DataInputStream in) {
        try{
            //request
            String s="GAME?"+END_TCP;
            byte[] request=s.getBytes();
            ot.write(request);
            ot.flush();
            //response
            int nbrGames;
            byte[] response=readFirstMessage(in);
            String responseSting= new String(response, StandardCharsets.UTF_8);
            if(responseSting.startsWith("GAMES ")){
                nbrGames=response[6];
                System.out.println("Le nombre de partie qui n'ont pas commencé avec avec des joueurs inscrit "+nbrGames);
                for (int i = 0; i < nbrGames; i++) {
                    int idGame, nbrPlayers;
                    byte[] secResponse=readMessage(in);
                    String secResponseSting= new String(secResponse, StandardCharsets.UTF_8);
                    if (secResponseSting.startsWith("OGAME ")){
                        idGame=secResponse[6];
                        nbrPlayers=secResponse[8];
                        System.out.println("La partie "+idGame+" possède "+nbrPlayers+"joueurs");
                    }else System.out.println(Error.responseServ);
                }
            }else System.out.println(Error.responseServ);
        } catch (IOException e) {
            System.out.println(Error.nbrArgs);
            e.printStackTrace();
        }
    }

}
