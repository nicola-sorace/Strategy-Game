#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <time.h> //TODO Remove (debugging)
#include <stdlib.h>
using namespace sf;
using namespace std;

#include "Interface.h"

#define TILES 2 //Number of tile types
#define TE 40

#define N 3 //Number of players in game + 1

#define LW 25 //Land width and height
#define LH 16

class Tile{
	public:
		Tile();
		Tile(int x, int y);
		int x,y;

		void draw(RenderWindow&, Font*, Color[N], Sprite[TILES], Sprite* tank, bool hover, bool selected, bool nearbyN, float px, float py, float z);
		//hover: Mouse is hovering over tile
		//nearbyN: New 'nearby' value 
		
		bool nearby; //Tile is adjecent to selected tile 
		
		void attack(Tile* t, int n);
		
		int type;
		bool mist; //Whether tile is discovered (unimplemented)
		CircleShape hexagon;
		int owner;
		int power; //Number of tanks on tile
		int powerN; //New power given current pending actions
};

#include "HUD.h"

class Land : public Interface{
	public:
		Land();
		void draw(RenderWindow&);
		void events(Event&, RenderWindow& window);
		Tile grid[LW][LH];
		Tile* hovered;
		Tile* selected;
		int player;

		//TODO Make this static
		Color pCols[N]; //Identifying color of each player (0th is no owner)
		string pNames[N]; //Name of each player

		HUD hud;

	private:
		float px; //Position x
		float py; //Position y
		float z;  //Scale

		Vector2i mouse;

		//Image map;
		//Texture mapT;
		//Sprite mapS;

		Image tiles[TILES];
		Texture tilesT[TILES];
		Sprite tilesS[TILES];

		Image tank;
		Texture tankT;
		Sprite tankS;

		Font font;
};


