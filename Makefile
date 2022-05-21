all: generate

generate :server/server_files/server.c
	gcc -g -pthread -I server/include -o server_exe server/server_files/server.c server/server_files/funct_aux.c server/server_files/list.c server/server_files/game.c server/server_files/entity_position.c server/server_files/labyrinthe.c
	javac client/*.java
	
clean:
	rm  client/*.class
	rm server_exe
	
history:
	rm *.ser
