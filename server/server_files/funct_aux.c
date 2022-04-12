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

int new_game(int sock2,listElements *games){
    // recv suite de NEWPL_id_port***
    int taille = 2+SIZE_OF_END+8+4;
    char buffer[taille];
    if(taille != recv(sock2,buffer,taille,0)){
        if(func_send_regno(sock2)==-1)perror("Couldn't send regno");
        return -1;
    }
    // création du joueur
    joueur *j1 = malloc(sizeof(joueur));
    memmove(j1->id,buffer+1,8);
    memmove(j1->port,buffer+10,4);
    j1->score=0;
    j1->x=-1;
    j1->y=-1;

    // création de la partie
    game *new = malloc(sizeof(game));
    new->encours=-1;
    new->game_id=games->count+1;
    addEl(new->joueurs,NULL,j1);
    addEl(games,games->last,new);

    char message[SIZE_OF_END+SIZE_OF_HEAD];
    memmove(message,REGOK,SIZE_OF_HEAD);
    memmove(message+SIZE_OF_HEAD,END_TCP,SIZE_OF_END);
    if((SIZE_OF_END+SIZE_OF_HEAD)!=send(sock2,message,SIZE_OF_END+SIZE_OF_HEAD,0)){
        return -1;
    }
    return 0;
}

int register_game(int sock2,listElements *games){
    // recv suite de REGIS_id_port_m***
    int taille = 3+SIZE_OF_END+8+4+sizeof(uint8_t);
    char buffer[taille];
    if(taille != recv(sock2,buffer,taille,0)){
        if(func_send_regno(sock2)==-1)perror("Couldn't send regno");
        return -1;
    }
    uint8_t id =*(uint8_t*)(buffer+15);
    game *_game = get_game(games,id);
    if(!_game){
        func_send_regno(sock2);
        return 0;
    }
    // création du joueur
    joueur *j1 = malloc(sizeof(joueur));
    memmove(j1->id,buffer+1,8);
    memmove(j1->port,buffer+10,4);
    j1->score=0;
    j1->x=-1;
    j1->y=-1;
    addEl(_game->joueurs,_game->joueurs->last,j1);

    char message[SIZE_OF_END+SIZE_OF_HEAD];
    memmove(message,REGOK,SIZE_OF_HEAD);
    memmove(message+SIZE_OF_HEAD,END_TCP,SIZE_OF_END);
    if((SIZE_OF_END+SIZE_OF_HEAD)!=send(sock2,message,SIZE_OF_END+SIZE_OF_HEAD,0)){
        return -1;
    }
    return 0;
}

int func_send_dunno(int sock2)
{
    int taille = SIZE_OF_HEAD+SIZE_OF_END;
    char message[taille];
    memmove(message,DUNNO,SIZE_OF_HEAD);
    memmove(message+SIZE_OF_HEAD,END_TCP,SIZE_OF_END);
    int count = send(sock2,message,taille,0);
    if(count!=taille){
        return -1;
    }
    return 0;
}

int func_send_regno(int sock2){
    int taille = SIZE_OF_HEAD+SIZE_OF_END;
    char message[taille];
    memmove(message,REGNO,SIZE_OF_HEAD);
    memmove(message+SIZE_OF_HEAD,END_TCP,SIZE_OF_END);
    int count = send(sock2,message,taille,0);
    if(count!=taille){
        return -1;
    }
    return 0;
}