#ifndef SERVER
#define SERVER

#include "game.h"
#include "funct_aux.h"
#include "joueur.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define SIZE_OF_HEAD 5
#define SIZE_OF_END 3
#define END_TCP "***"
// commandes server
#define GAMES "GAMES"
#define OGAME "OGAME"
#define REGOK "REGOK"
#define REGNO "REGNO"
#define DUNNO "DUNNO"
#define UNROK "UNROK"
#define LISTS "LIST!"
#define SIZES "SIZE!"
#define PLAYR "PLAYR"
// commandes client
#define NEWPL "NEWPL"
#define REGIS "REGIS"
#define IQUIT "IQUIT"
#define START "START"
#define UNREG "UNREG"
#define SIZEC "SIZE?"
#define LISTC "LIST?"
#define GAMEC "GAME?"
void *client_thread(void *socket);

#endif
