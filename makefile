# Makefile 
#
# CPSC 441, Assignment 2
# Glenn Skelton

CC = gcc
CFLAGS = -Wall -g
SRC = cryptoTweetServer.c cryptoTweetClient.c process.c hash.c
OBJ = $(SRC:.c=.o)



# Build rules for client and sever


all: client server

cryptoTweetServer.o: src/cryptoTweetServer.c src/definitions.h src/process.h src/hash.h
	$(CC) $(CFLAGS) -c src/cryptoTweetServer.c
	mv cryptoTweetServer.o artifacts

cryptoTweetClient.o: src/cryptoTweetClient.c src/definitions.h
	$(CC) $(CFLAGS) -c src/cryptoTweetClient.c
	mv cryptoTweetClient.o artifacts

process.o: src/process.c src/definitions.h src/process.h src/hash.h
	$(CC) $(CFLAGS) -c src/process.c
	mv process.o artifacts

hash.o: src/hash.c src/definitions.h src/hash.h
	$(CC) $(CFLAGS) -c src/hash.c
	mv hash.o artifacts

server: cryptoTweetServer.o process.o hash.o
	$(CC) artifacts/cryptoTweetServer.o artifacts/process.o artifacts/hash.o -o $@

client: cryptoTweetClient.o
	$(CC) artifacts/cryptoTweetClient.o -o $@

clean:
	@echo Destroying $(OBJ) 
	rm -r artifacts/*.o server client

.PHONY: clean
