package client;

import java.io.DataOutputStream;

public interface MessageUserPlayer{

    default void inRangeUint8(int value) throws IndexOutOfBoundsException{
        if (value>255 || value<0){
            throw new IndexOutOfBoundsException();
        }
    }

    //[SIZE? m***]
    void sizeMaze(int idMap);

    //[LIST? m***]
    void listPlayer(int idMap);

    //[GAME?***]
    void listGame();

}
