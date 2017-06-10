#include "Land.h"

Tile::Tile(){
}

Tile::Tile(int nx, int ny){
	x = nx;
	y = ny;
	//type = rand()%2;
	//mist = false;
	//owner = rand()%N;
	//power = owner&&type==0 ? rand()%8 : 0;
	//powerN = power;
}

void Tile::draw(RenderWindow& window, Font* font, Color pCols[N], Sprite sprites[TILES], Sprite* tank, bool hover, bool selected, bool nearbyN, float px, float py, float z){

	if(type==1 && power==0)owner=0; //Empty water is not owned
	nearby = nearbyN;

	hexagon = CircleShape(TE*z, 6);

	pCols[owner].a=255;

	sprites[type].setPosition(Tile::getX(x,y,px,z), Tile::getY(y,py,z));

	sprites[type].setScale(0.32*z,0.32*z);
	sprites[type].setColor(pCols[owner]);
	window.draw(sprites[type]);


	//tank->setColor(pCols[owner]);
	for(int i=0; i<min(3,power); i++){
		tank->setPosition((x*TE*0.86*2+i*2+10 - (y%2)*(TE*0.86) - px)*z, (y*TE*1.49+i*17+7 - py)*z); //TODO What is 0.86 and 1.5?
		tank->setScale(0.3*z,0.3*z);
		window.draw(*tank);
	}
	for(int i=3; i<min(6,power); i++){
		tank->setPosition((x*TE*0.86*2+i*2+30 - (y%2)*(TE*0.86) - px)*z, (y*TE*1.49+i*17-42 - py)*z); //TODO What is 0.86 and 1.5?
		tank->setScale(0.3*z,0.3*z);
		window.draw(*tank);
	}

	hexagon.setPosition((x*TE*0.86*2 - (y%2)*(TE*0.86) - px)*z, (y*TE*1.49 - py)*z); //TODO What is 0.86 and 1.5?
	//Vector2f d = hexagon.getPosition()+Vector2f(hexagon.getRadius(),hexagon.getRadius()) - static_cast<Vector2f>(Mouse::getPosition(window));
	//if( sqrt(pow(d.x,2)+pow(d.y,2))< hexagon.getRadius()){
	if(hover||selected||nearby){
		pCols[owner].a = (hover+selected*2+nearby) * (255/3) - 20; //selected ? 200 : 100;
		hexagon.setFillColor(pCols[owner]);//Color(0,0,100,100));

		window.draw(hexagon);

		if(power>0){
			//String pS(to_string(power));
			Text pN(to_string(powerN), *font);
			pN.setCharacterSize(30*z);
			pN.setPosition(((x+0.5)*TE*0.86*2 - (y%2)*(TE*0.86) - px + 2)*z - pN.getLocalBounds().width/2, (y*TE*1.49 + (power == powerN ? 20 : 10) - py)*z);
			window.draw(pN);
			if(power != powerN){
				Text p("("+to_string(power)+")", *font);
				p.setCharacterSize(15*z);
				p.setPosition(((x+0.5)*TE*0.86*2 - (y%2)*(TE*0.86) - px + 2)*z - p.getLocalBounds().width/2, (y*TE*1.49+40 - py)*z);
				window.draw(p);
			}
		}
	}
}

void Tile::attack(Tile* t, int n){
	if(t->owner == owner){
		power -= n;
		t->power += n;
	}else{
		if(t->power < n){ //TODO Probability?
			t->owner = owner;
			power -= n;
			t->power = n - t->power;

		}else{
			power -= n;
			t->power -= n;
		}
	}
	powerN = power; //TODO Should techically only occur on tile's last action
	t->powerN = t->power;
}

float Tile::getX(int x, int y, float px, float z){ //TODO Replaced all appropriate fixed expressions with these functions
	return (x*TE*0.86*2 - (y%2)*(TE*0.86) - px)*z;
}

float Tile::getY(int y, float py, float z){
	return (y*TE*1.49 - py)*z;
}
