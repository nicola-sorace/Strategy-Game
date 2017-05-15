CFLAGS=$(shell pkg-config --cflags)
LDFLAGS=$(shell pkg-config --libs sfml-graphics)
CC=g++

NAME=game.bin

build:
	$(CC) Main.cpp Interface.cpp Land.cpp Tile.cpp HUD.cpp $(CFLAGS) $(LDFLAGS) -o game.bin
