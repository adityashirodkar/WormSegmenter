CC = g++

CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all:
	$(CC) $(CFLAGS) main.cpp $(LIBS) -o main

clean:
	rm -rf main main.o

