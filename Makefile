all: server/server_files/server.c 
	gcc -pthread -I server/include -o server_exe server/server_files/server.c server/server_files/funct_aux.c server/server_files/list.c server/server_files/game.c server/server_files/entity_position.c server/server_files/labyrinthe.c
