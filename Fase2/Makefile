CC = gcc
CFLAGS = -Wall -g 
LIBS = -lpthread

ep1 : fase2.o fase2.c output.o 
	$(CC) $(CFLAGS) fase2.o -o ep1 $(LIBS)

fase2.o : fase2.c output.o 
	$(CC) $(CFLAGS) -c fase2.c $(LIBS)

output.o : output.h output.c lista.h
	$(CC) $(CFLAGS) -c output.c

lista.o : lista.h lista.c
	$(CC) $(CFLAGS) -c lista.o

clean :
	rm *.o
