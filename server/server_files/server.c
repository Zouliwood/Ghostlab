#include "server.h"

// déclaration des variables
listElements *games; // liste des parties partie->count pour le nombre de parties
pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
    games = malloc(sizeof(listElements));
    games->count = 0;
    games->first = NULL;
    games->last = NULL;
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address_sock;
    address_sock.sin_family = AF_INET;
    if (argc > 1)
    {
        int port = atoi(argv[1]);
        address_sock.sin_port = htons(port);
    }
    else
    {
        address_sock.sin_port = htons(6969);
    }
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
                free(sock2);
            }
        }
    }
    close(sock);
    free(games);
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
            if (me != NULL)
            {
                func_send_dunno(sock2);
                printf("52 Dunno\n");
            }
            else
                me = new_game(sock2, games);
        }
        else if (strcmp(command, REGIS) == 0)
        {
            if (me != NULL)
            {
                func_send_dunno(sock2);
                printf("62 Dunno\n");
            }
            else
                me = register_game(sock2, games);
        }
        else if (strcmp(command, START) == 0)
        {
            printf("Start game !\n");
            start_game(me, sock2);
            break;
        }
        else if (strcmp(command, UNREG) == 0)
        {
            char end[SIZE_OF_END];
            int r = recv(sock2, end, SIZE_OF_END, 0);
            if (me == NULL || r != SIZE_OF_END)
            {
                func_send_dunno(sock2);
                printf("75 Dunno\n");
            }
            else
            {
                me = func_unreg(me, games, sock2);
            }
        }
        else if (strcmp(command, SIZEC) == 0)
        {
            get_size_map(sock2, games);
        }
        else if (strcmp(command, GAMEC) == 0)
        {
            char end[SIZE_OF_END + 1];
            int r = recv(sock2, end, SIZE_OF_END, 0);
            end[r] = '\0';
            if (strcmp(END_TCP, end) == 0)
            {
                func_send_games(sock2, games);
            }
            else
            {
                func_send_dunno(sock2);
                printf("103 Dunno\n");
            }
        }
        else if (strcmp(command, LISTC) == 0)
        {
            char buffer[SIZE_OF_END + 1 + sizeof(uint8_t)];
            if (4 + sizeof(uint8_t) != recv(sock2, buffer, SIZE_OF_END + 1 + sizeof(uint8_t), 0))
            {
                func_send_dunno(sock2);
                printf("111 Dunno\n");
            }
            else
            {
                uint8_t m = (uint8_t)buffer[1];
                game *ptr = get_game(games, m);
                if (ptr == NULL)
                {
                    printf("109 Dunno\n");
                    func_send_dunno(sock2);
                }
                else
                    send_list(sock2, ptr);
            }
        }
        else
        {
            func_send_dunno(sock2);
            printf("118 Dunno\n");
        }
    }
    printf("I'm waiting\n");
    while (lockGameStatus(me->current) == -1)
    {
        // do nothing
    }
    if (lockGameStatus(me->current) == 1)
    {
        send_welco(sock2, me);
        while (lockGameStatus(me->current) == 1)
        {
            char command[SIZE_OF_HEAD + 1];
            int count = recv(sock2, command, SIZE_OF_HEAD, 0);
            command[count] = '\0';
            printf("count= %d and command= %s\n", count, command);
            if (strcmp(UPMOV, command) == 0)
            {
                movPlayer(sock2, 0, me, games);
            }
            else if (strcmp(DOMOV, command) == 0)
            {
                movPlayer(sock2, 2, me, games);
            }
            else if (strcmp(LEMOV, command) == 0)
            {
                movPlayer(sock2, 3, me, games);
            }
            else if (strcmp(RIMOV, command) == 0)
            {
                movPlayer(sock2, 1, me, games);
            }
            else if (strcmp(IQUIT, command) == 0)
            {
                // on vérifie que le protocole est respecté
                int taille = SIZE_OF_END;
                char buffer[taille + 1];
                if (taille != recv(sock2, buffer, taille, 0))
                {
                    func_send_dunno(sock2);
                    printf("%d\n", taille);
                }
                buffer[3] = '\0';
                if (strcmp(buffer, END_TCP) != 0)
                {
                    func_send_dunno(sock2);
                    printf("not end tcp %s\n", buffer);
                }
                quit_game(sock2, me, games);
                me=NULL;
                break;
            }
            else if (strcmp(GLISC, command) == 0)
            {
                char end[SIZE_OF_END + 1];
                int r = recv(sock2, end, SIZE_OF_END, 0);
                end[r] = '\0';
                if (r != SIZE_OF_END || strcmp(end, END_TCP) != 0)
                {
                    func_send_dunno(sock2);
                    printf("75 Dunno\n");
                }
                else
                    send_glis(sock2, me);
            }
            else if (strcmp(MALLC, command) == 0)
            {
                // MALL?
                send_all(sock2,me);
            }
            else if (strcmp(SENDC, command) == 0)
            {
                // SEND?_id_mess
                sendc(sock2,sendMess(sock2, me));
            }
            else
            {
                func_send_dunno(sock2);
            }
            printf("Fantome en jeu %d\n",getListCount(me->current->fantomes));
        }
    }
    if (me != NULL)
    {
        char poubelle[300];
        recv(sock2, poubelle, 300, 0);
        quit_game(sock2, me, games);
    }
    close(sock2);
    return NULL;
}