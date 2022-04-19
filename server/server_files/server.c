#include "server.h"

// déclaration des variables
listElements *games; // liste des parties partie->count pour le nombre de parties

int main(int argc, char const *argv[])
{
    games = malloc(sizeof(listElements));
    games->count = 0;
    games->first = NULL;
    games->last = NULL;
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address_sock;
    address_sock.sin_family = AF_INET;
    address_sock.sin_port = htons(6969);
    address_sock.sin_addr.s_addr = htonl(INADDR_ANY);
    int r = bind(sock, (struct sockaddr *)&address_sock, sizeof(struct sockaddr_in));
    if (r == 0)
    {
        r = listen(sock, 0);
        while (1)
        {
            struct sockaddr_in caller;
            socklen_t size = sizeof(caller);
            int *sock2 = (int *)malloc(sizeof(int));
            *sock2 = accept(sock, (struct sockaddr *)&caller, &size);
            pthread_t client;
            if (pthread_create(&client, NULL, client_thread, sock2) != 0)
            {
                printf("Couldn't create threads");
                close(sock);
            }
        }
    }
    return 0;
}

void *client_thread(void *socket)
{
    int sock2 = *((int *)socket);
    func_send_games(sock2, games);
    joueur *me = NULL;
    while (1)
    {
        char command[SIZE_OF_HEAD + 1];
        int count = recv(sock2, command, SIZE_OF_HEAD, 0);
        command[count] = '\0';
        printf("count= %d and command= %s\n", count, command);
        if (strcmp(command, NEWPL) == 0)
        {
            printf("creating game\n");
            me = new_game(sock2, games);
            printf("I'm %s\n", me->id);
        }
        else if (strcmp(command, REGIS) == 0)
        {
            printf("registering game\n");
            me = register_game(sock2, games);
        }
        else if (strcmp(command, START) == 0)
        {
            printf("start player \n");
            start_game(me, sock2);
            break;
        }
        else if (strcmp(command, UNREG) == 0)
        {
            if (me == NULL)
                func_send_dunno(sock2);
            else
                me = func_unreg(me, sock2);
        }
        else if (strcmp(command, SIZEC) == 0)
        {
        }
        else if (strcmp(command, GAMEC) == 0)
        {
            char end[SIZE_OF_END+1];
            int r = recv(sock2, end, SIZE_OF_END, 0);
            end[r]='\0';
            if (strcmp(END_TCP, end) == 0)
            {
                func_send_games(sock2, games);
            }
            else func_send_dunno(sock2);
        }
        else if (strcmp(command, LISTC) == 0)
        {
            char buffer[1 + sizeof(uint8_t)];
            if (1 + sizeof(uint8_t) != recv(sock2, buffer, 1 + sizeof(uint8_t), 0))
            {
                func_send_dunno(sock2);
            }
            uint8_t m = *(uint8_t *)buffer + 1;
            game *ptr = get_game(games, m);
            if (ptr == NULL)
            {
                func_send_dunno(sock2);
            }
            else
                send_list(sock2, ptr);
        }
        else
        {
            func_send_dunno(sock2);
        }
    }

    while (me->current->encours == 1)
    {
        /* commande dans la game */
    }

    close(sock2);
    return NULL;
}