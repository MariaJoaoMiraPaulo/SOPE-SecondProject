CC = gcc
CFLAGS = -D_REENTRANT -lpthread -Wall

all: Parque Gerador

Parque: Parque.c
	$(CC) Parque.c -o Parque $(CFLAGS)

Gerador: Gerador.c
	$(CC) Gerador.c -o Gerador $(CFLAGS)

clean:
	rm Gerador
	rm Parque
