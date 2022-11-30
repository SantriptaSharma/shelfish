CC = gcc
CCFLAGS = -pthread
SRC = ./src
DST = ./dist

default: clean

clean:
	rm -rf ./dist/*

server.out: $(SRC)/server/*.c
	$(CC) $(CCFLAGS) -o $(DST)/$@ $^

client.out: $(SRC)/client/*.c
	$(CC) $(CCFLAGS) -o $(DST)/$@ $^

client: client.out
server: server.out

.phony: all default clean client server