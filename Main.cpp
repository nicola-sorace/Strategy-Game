#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

#include "Land.h"

int main(){
	ContextSettings settings;
	settings.antialiasingLevel = 8;

	RenderWindow window(VideoMode::getFullscreenModes()[0], "Strategy Game", Style::Fullscreen, settings);
	window.setFramerateLimit(24);

	Interface* interface = new Land();
	
	while(window.isOpen()){

		Event event;
		while(window.pollEvent(event)){
			if(event.type == Event::Closed) window.close();
			interface->events(event, window);
		}

		interface->draw(window);

		window.display();
	}

	return 0;
}
