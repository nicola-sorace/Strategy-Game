CFLAGS=$(shell pkg-config --cflags)
LDFLAGS_G=$(shell pkg-config --libs sfml-graphics)
LDFLAGS_S=$(shell pkg-config --libs sfml-network)
LDFLAGS=$(shell pkg-config --libs sfml-graphics sfml-network)
CC=g++

NAME=game.bin

all: build_game build_server

build_game:
	$(CC) -g Main.cpp Interface.cpp Land.cpp Tile.cpp HUD.cpp $(CFLAGS) -lpthread $(LDFLAGS) -o game

build_server:
	$(CC) -g Server.cpp $(CFLAGS) -lpthread $(LDFLAGS_S) -o server
