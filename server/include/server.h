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
// commandes client
#define NEWPL "NEWPL"
#define REGIS "REGIS"
#define IQUIT "IQUIT"
void *client_thread(void *socket);

#endif
