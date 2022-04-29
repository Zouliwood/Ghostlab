#ifndef func_aux
#define func_aux
#include "list.h"
#include "server.h"
#include "joueur.h"
#include "entity_position.h"
#include "labyrinthe.h"

// NEWPL
joueur* new_game(int sock2,listElements *games);
// REGIS
joueur* register_game(int sock2,listElements *games);
// GAMES + 0GAME
void func_send_games(int sock2,listElements *games);
// DUNNO
void func_send_dunno(int sock2);
// REGNO
void func_send_regno(int sock2);
// START
void start_game(joueur *joueur,int socket);
// UNREG
void *func_unreg(joueur *joueur, listElements *games, int sock);
// LIST! + PLAYR
void send_list(int sock, game *game_current);
// SIZE?
void get_size_map(int sock, listElements* games);
// WELCO
void send_welco(int sock,joueur *joueur);
// POSIT
void send_posit(int sock,joueur *joueur);
// MOV
void movPlayer(int sock,int direction,joueur *joueur);
// IQUIT
void quit_game(int sock, joueur *player, listElements* games);

#endif
