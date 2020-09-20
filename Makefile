CC = gcc
CFLAGS = -Wall -g 
LIBS = -lreadline

bccsh : fase1.o fase1.c
	$(CC) $(CFLAGS) fase1.o -o bccsh $(LIBS)

fase1.o : fase1.c
	$(CC) $(CFLAGS) -c fase1.c $(LIBS)

clean :
	rm *.o
