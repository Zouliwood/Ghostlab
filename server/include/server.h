#ifndef SERVER
#define SERVER

#include "game.h"
#include "funct_aux.h"
#include "joueur.h"
#include "labyrinthe.h"
#include "fantome.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
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
#define MOVES "MOVE!"
#define MOVEF "MOVEF"
#define WELCO "WELCO"
#define POSIT "POSIT"
#define GLISS "GLIS!"
#define GPLYR "GPLYR"
#define GOBYE "GOBYE"
// commandes client
#define NEWPL "NEWPL"
#define REGIS "REGIS"
#define IQUIT "IQUIT"
#define START "START"
#define UNREG "UNREG"
#define SIZEC "SIZE?"
#define LISTC "LIST?"
#define GAMEC "GAME?"
#define UPMOV "UPMOV"
#define DOMOV "DOMOV"
#define LEMOV "LEMOV"
#define RIMOV "RIMOV"
#define GLISC "GLIS?"
#define MALLC "MALL?"
#define SENDC "SEND?"
void *client_thread(void *socket);

#endif
