CC = gcc
CFLAGS = -D_REENTRANT -lpthread -lrt -Wall

all: Parque Gerador

Parque: Parque.c
	$(CC) Parque.c -o Parque $(CFLAGS)

Gerador: Gerador.c
	$(CC) Gerador.c -o Gerador $(CFLAGS)

clean:
	rm Gerador
	rm Parque
