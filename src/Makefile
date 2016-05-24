CC = gcc
CFLAGS = -D_REENTRANT -lpthread -Wall -pthread

all: clear Parque Gerador

Parque: Parque.c
	$(CC) Parque.c -o bin/Parque $(CFLAGS)

Gerador: Gerador.c
	$(CC) Gerador.c -o bin/Gerador $(CFLAGS)

clean:
	rm Gerador
	rm Parque

clear:
	clear
