#include "Land.h"

Tile::Tile(){
}

Tile::Tile(int nx, int ny){
	x = nx;
	y = ny;
	type = rand()%2;
	mist = false;
	owner = rand()%N;
	power = owner&&type==0 ? rand()%8 : 0;
	powerN = power;
}

void Tile::draw(RenderWindow& window, Font* font, Color pCols[N], Sprite sprites[TILES], Sprite* tank, bool hover, bool selected, bool nearbyN, float px, float py, float z){

	if(type==1 && power==0)owner=0; //Empty water is not owned
	nearby = nearbyN;

	/*
	ConvexShape hexagon;
	hexagon.setPointCount(6);
	const float s = 1.f*100.f;
	const float h = 0.5f*100.f;             //sin(30°)
	const float r = 0.86602540f*100.f;      //cos(30°)
	hexagon.setPoint(0, Vector2f(0.f, -s));
	hexagon.setPoint(1, Vector2f( -r, -h));
	hexagon.setPoint(2, Vector2f( -r, +h));
	hexagon.setPoint(3, Vector2f(0.f, +s));
	hexagon.setPoint(4, Vector2f( +r, +h));
	hexagon.setPoint(5, Vector2f( +r, -h));
	hexagon.scale(0.5f,0.5f);
	*/

	hexagon = CircleShape(TE*z, 6);
	
	/*	
	ConvexShape hexagon;
	hexagon.setPointCount(6);
	hexagon.setPoint(0, Vector2f(0.f, 0.f));
	hexagon.setPoint(1, Vector2f(TE, 0.f));
	hexagon.setPoint(2, Vector2f(TE+TE/2, TE/2));
	hexagon.setPoint(3, Vector2f(TE, TE));
	hexagon.setPoint(4, Vector2f(0.f, TE));
	hexagon.setPoint(5, Vector2f(-TE/2, TE/2));
	*/

	/*
	if(!mist){
		if(type==0)hexagon.setFillColor(Color(0,0,0,0));
		else hexagon.setFillColor(Color(255,0,0,50));
		hexagon.setOutlineThickness(1);
		hexagon.setOutlineColor(Color(255,255,255,50));
	}else{
		hexagon.setFillColor(Color(255,255,255,200));
		hexagon.setOutlineThickness(1);
		hexagon.setOutlineColor(Color(255,255,255,200));
	}
	hexagon.setPosition((x*TE*0.87*2 - (y%2)*(TE*0.87) - px)*z, (y*TE*1.5 - py)*z);
	//window.draw(hexagon);
	*/	

	pCols[owner].a=255;

	sprites[type].setPosition((x*TE*0.86*2 - (y%2)*(TE*0.86) - px)*z, (y*TE*1.49 - py)*z);

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
		if(t->power < n){ //TODO Probability (and attacker losses)
			t->owner = owner;
			power -= n;
			t->power = n;

		}else{
			power -= n;
			t->power -= n;
		}
	}
}
