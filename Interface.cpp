#include <math.h>
#include "Interface.h"

Interface::Interface(){
	/*
	texture.loadFromFile("assets/img.png");
	texture.setRepeated(true);
	sprite = Sprite(texture);
	sprite.setTextureRect({0,0,1920,1080});
	t=0.0f;
	*/
}

void Interface::draw(RenderWindow& window){
	window.clear(Color(0,0,0));
	/*
	window.draw(sprite);
	
	RectangleShape shade(Vector2f(1920,1080));
	shade.setFillColor(Color(0,0,0,(sin(t)/2+0.5)*255));
	window.draw(shade);
	t+=0.05;
	*/
}

void Interface::events(Event& event, RenderWindow& window){
	
}
