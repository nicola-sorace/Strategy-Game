#include "Land.h"

#define ATTW 300 //Width and height of the moving units dialogue box
#define ATTH 200

HUD::HUD(){
	n = 0;
	selected = NULL;
	target = NULL;
}

void HUD::draw(RenderWindow& window, Font* font, Color pCols[N], std::string pNames[N], int player, Tile* s, float px, float py, float z){

	if(s == NULL){
		selected = NULL;
		target = NULL;
	}

	Vector2u size = window.getSize();

	RectangleShape header(Vector2f(size.x, 50));
	header.setPosition(0,0);
	pCols[player].a = 200;
	header.setFillColor(pCols[player]);
	window.draw(header);

	Text playerInfo("Playing as: "+pNames[player], *font);
	playerInfo.setCharacterSize(30);
	playerInfo.setPosition(10,5);
	window.draw(playerInfo);

	if(target != NULL){
		int x = ((target->x+1)*TE*0.86*2 - (target->y%2)*(TE*0.86) - px)*z;
		int y = ((target->y+1)*TE*1.49 - py)*z;
		bool friendly = target->owner == selected->owner;

		moveBox = RectangleShape(Vector2f(ATTW, ATTH));
		moveBox.setPosition(x, y);
		pCols[player].a = 200;
		moveBox.setFillColor(pCols[player]);
		moveBox.setOutlineThickness(2);
		moveBox.setOutlineColor(Color(26,26,26,250));
		window.draw(moveBox);

		Text moveTitle(friendly ? "Moving" : "Attacking", *font);
		moveTitle.setPosition(x+(ATTW-moveTitle.getLocalBounds().width)/2, y+5);
		window.draw(moveTitle);
		
		if(friendly||true){ //TODO Attack
			Text numberP(std::to_string(n), *font); 
			numberP.setCharacterSize(100);
			numberP.setPosition(x+(ATTW-numberP.getLocalBounds().width)/2, y+40);
			window.draw(numberP);
		}
	}
	
}

void HUD::events(Event& event, RenderWindow& window){
	switch(event.type){
		case Event::MouseWheelScrolled:
			if((event.mouseWheelScroll.delta>0||n>0) && (event.mouseWheelScroll.delta<0||n<selected->power)){
				n += event.mouseWheelScroll.delta;
			}
			break;
		case Event::MouseButtonReleased:
			if(event.mouseButton.button == Mouse::Right){
				target = NULL;
				selected = NULL;
			}
			if(event.mouseButton.button == Mouse::Left){
				selected->attack(target, n);
				target = NULL;
				selected = NULL;
			}
			break;
		default:
			break;
	}
}

void HUD::setTiles(Tile* t, Tile* s){
	target = t;
	selected = s;

	n = s->power;
}

bool HUD::isBusy(int x, int y, float px, float py, float z){
	if(target != NULL)return moveBox.getGlobalBounds().contains(Vector2f(x,y)); //TODO Possible segfault on moveBox
	
}
