#include "Land.h"

Land::Land() : Interface(){
	srand (time(NULL));

	hovered = NULL;
	selected = NULL;

	player = 1;

	px = -100;
	py = -20;
	z = 1;
	
	tpx = px;
	tpy = py;
	tz = z;

	mouse = Vector2i(-1,-1);

	for(int x=0; x < LW; x++){
		for(int y=0; y < LH; y++){
			grid[x][y] = Tile(x,y);
		}
	}

	//map = Image();
	//map.loadFromFile("assets/earth.jpg");

	for(int i=0; i<TILES; i++){
		tiles[i] = Image();
		tiles[i].loadFromFile("assets/tiles/"+to_string(i)+".png");
	}

	tank = Image();
	tank.loadFromFile("assets/tank.png");

	font.loadFromFile("assets/Roboto-Regular.ttf");

	hud = HUD();

	curAct = 0;
	anim = false;
	clock = Clock();
	doneAction = false;
}

void Land::draw(RenderWindow& window){
	Interface::draw(window);

	Color pCols[] = {Color(255,255,255),Color(100,100,255),Color(255,150,150)};
	string pNames[] = {"none", "Blue", "Red"};

	px += (tpx - px)/TSTEPS;
	py += (tpy - py)/TSTEPS;
	z += (tz - z)/TSTEPS;



	if(Mouse::isButtonPressed(Mouse::Middle)){
		if(mouse != Vector2i(-1,-1)){
			px = px - (Mouse::getPosition(window).x - mouse.x)/z;
			py = py - (Mouse::getPosition(window).y - mouse.y)/z;

			tpx = px;
			tpy = py;
		}	
		mouse = Mouse::getPosition(window);
	}else{
		mouse = Vector2i(-1,-1);
	}

	for(int i=0; i<TILES; i++){
		tilesT[i] = Texture();
		tilesT[i].loadFromImage(tiles[i]);
		tilesS[i] = Sprite(tilesT[i]);
	}

	tankT = Texture();
	tankT.loadFromImage(tank);
	tankS = Sprite(tankT);

	for(int x=0; x < LW; x++){
		for(int y=0; y < LH; y++){
			Tile* t = &grid[x][y];
			Vector2f d = t->hexagon.getPosition()+Vector2f(t->hexagon.getRadius(),t->hexagon.getRadius()) - static_cast<Vector2f>(Mouse::getPosition(window)); //TODO More accurate method.
			bool hover = sqrt(pow(d.x,2)+pow(d.y,2)) < t->hexagon.getRadius();
			if(hover)hovered = t;

			bool nearby = selected!=NULL && selected!=t && pow(t->x-selected->x,2)+pow(t->y-selected->y,2) < 4 && (t->y == selected->y || (t->y%2==0 ? t->x <= selected->x : t->x >= selected->x)); 

			t->draw(window, &font, pCols, tilesS, &tankS, hover, selected==t, nearby, px, py, z);
		}
	}

	if(anim){
		Int32 t = clock.getElapsedTime().asMilliseconds();
		action* a = &hud.actions[curAct];
		if(t<=ATICK && a->from != NULL){
			//Show tile before move
			tpx = (a->from->x+0.5)*TE*0.86*2-window.getSize().x/(2*z); //Note that these are mere approximations
			tpy = (a->from->y+2)*TE*1.49-window.getSize().y/(2*z);
		}else if(t<=2*ATICK){
			//TODO Show move
			//void drawAction(int a,int n,RenderWindow&, Font*, Color[N], string[N], int player, Tile* s, float px, float py, float z);
			hud.drawAction(curAct,a->n,window,&font,pCols,pNames,player,selected,px,py,z);
		}else if(t<=3*ATICK){
			//TODO Show tiles after move
			if(!doneAction){
				if(a->from != NULL){
					a->from->attack(a->to, a->n);
				}else{
					//TODO Here goes code for creating power
					cout << "SHIIITTTWHAAT???";
				}
				doneAction = true;
			}
		}else if(!doneAction){ //In case of extreme lag, action may have not executed
			if(a->from != NULL){
				a->from->attack(a->to, a->n);
			}else{
				//TODO Here goes code for creating power
			}

		}else if(curAct<hud.actions.size()-1){
			doneAction = false;
			curAct++;
			clock.restart();
		}else{
			doneAction = false;
			hud.actions.clear();
			anim = false;
		}
	}
	
	hud.draw(window, &font, this, pCols, pNames, player, selected, px, py, z);
}

void Land::events(Event& event, RenderWindow& window){
	if(!hud.isBusy(Mouse::getPosition(window).x, Mouse::getPosition(window).y, px, py, z)){
		switch(event.type){
			case Event::MouseWheelScrolled:
				if((event.mouseWheelScroll.delta>0||z>1) && (event.mouseWheelScroll.delta<0||z<5)){
					z += z*event.mouseWheelScroll.delta*0.2;
					px += (event.mouseWheelScroll.x*event.mouseWheelScroll.delta*0.2)/z;
					py += (event.mouseWheelScroll.y*event.mouseWheelScroll.delta*0.2)/z;
					
					tz = z;
					tpx = px;
					tpy = py;
				}
				break;
			case Event::MouseButtonReleased:
				if(event.mouseButton.button == Mouse::Left && hovered != NULL){
					if(selected != hovered && selected == NULL && hovered->owner == player)selected = hovered;
					else if(hovered->nearby){
						//selected->attack(hovered, selected->power); //TODO Settable power (dialogue box)
						//selected = NULL;
						hud.setTiles(hovered, selected);
					}else selected = NULL;
				}
			default:
				break;
		}
	}else{
		hud.events(event, this, window);
		if(hud.selected == NULL)selected = NULL; //TODO likely to cause issues
	}
}

void Land::startAnim(){
	//TODO Other player's actions must be loaded in before this point
	
	curAct = 0;
	clock.restart();
	anim = true;
	tz = 2;
}
