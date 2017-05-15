#include <SFML/Graphics.hpp>

using namespace sf;

class Interface{
	public:
		Interface();
		virtual void draw(RenderWindow&);
		virtual void events(Event&, RenderWindow&);
		//Sprite sprite;	
	private:
		//Texture texture;
		float t;
};
