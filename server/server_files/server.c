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
                perror("Couldn't create threads");
                close(sock);
                exit(1);
            }
        }
    }
    close(sock);
    return 0;
}

void *client_thread(void *socket)
{
    int sock2 = *((int *)socket);

    if (func_send_games(sock2, games) == -1)
    {
        perror("Error while sending GAMES/ 0GAME");
    }
    char command[SIZE_OF_HEAD + 1];
    int count = recv(sock2, command, SIZE_OF_HEAD, 0);
    command[count] = '\0';
    if (strcmp(command, NEWPL))
    {
        if(new_game(sock2,games)==-1){
            perror("Error while creating new game");
        }
    }
    else if (strcmp(command, REGIS))
    {
    }
    else
    {
        if (func_send_dunno(sock2) == -1)
        {
            perror("Error while sending DUNNO");
        }
    }

    close(sock2);
    return NULL;
}