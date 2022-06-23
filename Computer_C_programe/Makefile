prog: main.c websockets.c encryption.c
	gcc main.c websockets.c encryption.c
prog1: main.o encryption.o websockets.o
	gcc main.o websockets.o
main.o: main.c
	gcc -c main.c
encryption.o: encryption.c
	gcc -c encryption.c
websockets.o: websockets.c
	gcc -c websockets.c
