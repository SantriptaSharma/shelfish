CC = gcc
CCFLAGS = -pthread -g3 -Wall
SRC = ./src
DST = ./dist

default: clean

clean:
	rm -rf ./dist/*

server.out: $(SRC)/server/*.c
	$(CC) $(CCFLAGS) $^ -o $(DST)/$@

client.out: $(SRC)/client/*.c
	$(CC) $(CCFLAGS) $^ -o $(DST)/$@

client: client.out
server: server.out
all: client server

.phony: all default clean client server