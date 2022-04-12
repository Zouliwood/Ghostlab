#ifndef func_aux
#define func_aux
#include "list.h"
#include "server.h"

int func_send_games(int sock2,listElements *games);
int func_send_dunno(int sock2);
int new_game(int sock2,listElements *games);
int func_send_regno(int sock2);
int register_game(int sock2,listElements *games);

#endif
