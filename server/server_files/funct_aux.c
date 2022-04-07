#include "funct_aux.h"

int func_send_games(int sock2,listElements *games)
{
    int size_games =sizeof(uint8_t)+SIZE_OF_HEAD+1+SIZE_OF_END+1;
    char games_mess[size_games];
    memmove(games_mess,GAMES,SIZE_OF_HEAD);
    memmove(games_mess+SIZE_OF_HEAD," ",1);
    memmove(games_mess+SIZE_OF_HEAD+1,&(games->count),sizeof(uint8_t));
    memmove(games_mess+SIZE_OF_HEAD+1+sizeof(uint8_t),END_TCP,SIZE_OF_END);
    if(size_games!=send(sock2,games_mess,size_games,0)){
        return -1;
    }

    element *ptr=games->first;
    for(int i = 0;i<games->count;i++){
        int taille = sizeof(uint8_t)*2+SIZE_OF_HEAD+2+SIZE_OF_END;
        char game_mess[taille+1];
        memmove(game_mess,OGAME,SIZE_OF_HEAD);
        memmove(game_mess+SIZE_OF_HEAD," ",1);
        memmove(game_mess+SIZE_OF_HEAD+1,&(((game *)ptr->data)->game_id),sizeof(uint8_t));
        memmove(game_mess+SIZE_OF_HEAD+1+sizeof(uint8_t)," ",1);
        memmove(game_mess+SIZE_OF_HEAD+2+sizeof(uint8_t),&(((game *)ptr->data)->joueurs->count),sizeof(uint8_t));
        memmove(game_mess+SIZE_OF_HEAD+2+(sizeof(uint8_t)*2),END_TCP,SIZE_OF_END);
        game_mess[taille]='\0';
        printf("%s\n",game_mess);
        if(size_games!=send(sock2,games_mess,size_games,0)){
            return -1;
        }
        ptr=ptr->next;
    }
    return 0;
}