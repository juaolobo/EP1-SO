CC = gcc
CFLAGS = -Wall -g 
LIBS = -lpthread
SRC = ./src

ep1 : $(SRC)/fase2.o $(SRC)/output.o 
	$(CC) $(CFLAGS) $(SRC)/fase2.o $(SRC)/output.o -o ep1 $(LIBS)

fase2.o : $(SRC)/fase2.c $(SRC)/output.o $(SRC)/output.h
	$(CC) $(CFLAGS) -c $(SRC)/fase2.c  $(LIBS)

output.o : $(SRC)/output.h $(SRC)/output.c 
	$(CC) $(CFLAGS) -c $(SRC)/output.c

clean :
	rm *.o
