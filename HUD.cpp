#include "Land.h"

#define ATTW 300 //Width and height of the moving units dialogue box
#define ATTH 200

#define PAVW 300 //Width and height of the pending actions view
#define PAVH 200

HUD::HUD() : Interface(){
	bar = RectangleShape();

	n = 0;
	selected = NULL;
	target = NULL;
	actionE = NULL;
	actionEPos = 0;

	moveBox = RectangleShape();

	actsScroll = 0;
	actsBounds = RectangleShape();
}

void HUD::drawAction(int a, int n, RenderWindow& window, Font* font, Color pCols[N], string pNames[N], int player, Tile* s, float px, float py, float z){

	int tx = actions[a].to->x;
	int ty = actions[a].to->y;

	if(actions[a].from != NULL){
		RectangleShape line;
		line.setFillColor(Color(250,250,250));
		//line.setOutlineThickness(2);
		//line.setOutlineColor(Color(26,26,26));
		CircleShape arrow(10,3);
		arrow.setOrigin(10,0);
		arrow.setFillColor(Color(250,250,250));
		//arrow.setOutlineThickness(2);
		//arrow.setOutlineColor(Color(26,26,26));

		RectangleShape pBox(Vector2f(20*z,20*z));
		pBox.setOrigin(10*z,10*z);
		//pBox.setOutlineThickness(2);
		//pBox.setOutlineColor(Color(26,26,26));
		pCols[player].a = 240;
		pBox.setFillColor(pCols[player]);
		Text pNum("0", *font);
		pNum.setCharacterSize(14*z);
		//pNum.setFillColor(Color(26,26,26));

		int fx = actions[a].from->x;
		int fy = actions[a].from->y;

		fx = ((fx+1)*TE*0.86*2 - (fy%2)*(TE*0.86) - px + 10)*z;
		fy = ((fy+1)*TE*1.49 - py + 20)*z;

		tx = ((tx+1)*TE*0.86*2 - (ty%2)*(TE*0.86) - px + 10)*z;
		ty = ((ty+1)*TE*1.49 - py + 20)*z;

		float o = ty-fy;
		float a = tx-fx;

		line.setSize(Vector2f(sqrt(pow(o,2)+pow(a,2)) - 10, 5));
		line.setOrigin(-5,2.5);
		line.setRotation((a<0?180:0)+atan(o/a)/(2*3.14159)*360);
		arrow.setRotation(line.getRotation()+90);
		line.setPosition(fx-TE*z,fy-TE*z);
		arrow.setPosition(tx-TE*z, ty-TE*z);

		window.draw(line);
		window.draw(arrow);

		pBox.setPosition((fx+a/2)-TE*z, (fy+o/2)-TE*z);
		window.draw(pBox);
		pNum.setString(to_string(n));
		pNum.setOrigin(pNum.getLocalBounds().width/2 + z, 9*z);
		pNum.setPosition((fx+a/2)-TE*z, (fy+o/2)-TE*z);
		window.draw(pNum);

	}else{

	}
}

void HUD::draw(RenderWindow& window, Font* font, Land* land, Color pCols[N], string pNames[N], int player, Tile* s, float px, float py, float z){

	if(s == NULL){
		selected = NULL;
		target = NULL;
		actionE = NULL;
	}

	Vector2u size = window.getSize();

	/*
	RectangleShape line;
	line.setFillColor(Color(250,250,250));
	//line.setOutlineThickness(2);
	//line.setOutlineColor(Color(26,26,26));
	CircleShape arrow(10,3);
	arrow.setOrigin(10,0);
	arrow.setFillColor(Color(250,250,250));
	//arrow.setOutlineThickness(2);
	//arrow.setOutlineColor(Color(26,26,26));

	RectangleShape pBox(Vector2f(20*z,20*z));
	pBox.setOrigin(10*z,10*z);
	//pBox.setOutlineThickness(2);
	//pBox.setOutlineColor(Color(26,26,26));
	pCols[player].a = 240;
	pBox.setFillColor(pCols[player]);
	Text pNum("0", *font);
	pNum.setCharacterSize(14*z);
	//pNum.setFillColor(Color(26,26,26));

	int fx, fy, tx, ty;
	float o,a; //Opposite, adjacent

	for(int i=0; i<actions.size(); i++){
		tx = actions[i].to->x;
		ty = actions[i].to->y;

		if(actions[i].from != NULL){
			fx = actions[i].from->x;
			fy = actions[i].from->y;

			fx = ((fx+1)*TE*0.86*2 - (fy%2)*(TE*0.86) - px + 10)*z;
			fy = ((fy+1)*TE*1.49 - py + 20)*z;

			tx = ((tx+1)*TE*0.86*2 - (ty%2)*(TE*0.86) - px + 10)*z;
			ty = ((ty+1)*TE*1.49 - py + 20)*z;

			o = ty-fy;
			a = tx-fx;

			line.setSize(Vector2f(sqrt(pow(o,2)+pow(a,2)) - 10, 5));
			line.setOrigin(-5,2.5);
			line.setRotation((a<0?180:0)+atan(o/a)/(2*3.14159)*360);
			arrow.setRotation(line.getRotation()+90);
			line.setPosition(fx-TE*z,fy-TE*z);
			arrow.setPosition(tx-TE*z, ty-TE*z);

			window.draw(line);
			window.draw(arrow);

			pBox.setPosition((fx+a/2)-TE*z, (fy+o/2)-TE*z);
			window.draw(pBox);
			pNum.setString(to_string(actions[i].n));
			pNum.setOrigin(pNum.getLocalBounds().width/2 + z, 9*z);
			pNum.setPosition((fx+a/2)-TE*z, (fy+o/2)-TE*z);
			window.draw(pNum);

		}else{

		}
	}
	*/
	if(land->anim == false){
		for(int i=0; i<actions.size(); i++)drawAction(i, actions[i].n, window,font,pCols,pNames,player,s,px,py,z);
	}

	bar.setSize(Vector2f(size.x, PAVH+60));
	bar.setPosition(0,size.y-bar.getLocalBounds().height);
	pCols[player].a = 240;
	bar.setFillColor(pCols[player]);
	//bar.setOutlineThickness(4);
	//bar.setOutlineColor(Color(255,255,255));
	window.draw(bar);

	Text barText("Playing as: "+pNames[player], *font);
	barText.setCharacterSize(30);
	barText.setPosition(10,size.y-50);
	window.draw(barText);

	actsBounds.setSize(Vector2f(PAVW, PAVH));
	actsBounds.setFillColor(Color(26,26,26));
	actsBounds.setPosition(size.x - PAVW - 10, size.y - PAVH - 10);
	window.draw(actsBounds);

	barText.setString("Pending Actions:");
	barText.setPosition(size.x-PAVW+(PAVW-barText.getLocalBounds().width)/2-10,size.y-PAVH-55);
	window.draw(barText);

	Text action("???", *font);
	action.setCharacterSize(12);
	int vLoc;
	for(int i=0; i<actions.size(); i++){
		vLoc = size.y - 25 - (actions.size()-i)*14 + actsScroll;
		if(vLoc>size.y-PAVH-10 && vLoc<size.y-30){
			int tx = actions[i].to->x;
			int ty = actions[i].to->y;

			if(actions[i].from != NULL){
				int fx = actions[i].from->x;
				int fy = actions[i].from->y;

				action.setString("Moving "+to_string(actions[i].n)+" from ("+to_string(fx)+","+to_string(fy)+") to ("+to_string(tx)+","+to_string(ty)+").");
			}else{
				action.setString("Adding "+to_string(actions[i].n)+" to ("+to_string(tx)+","+to_string(ty)+").");
			}
			action.setPosition(size.x - PAVW + 10, vLoc);
			window.draw(action);
		}
	}

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
			Text numberP(to_string(n), *font);
			numberP.setCharacterSize(100);
			numberP.setPosition(x+(ATTW-numberP.getLocalBounds().width)/2, y+40);
			window.draw(numberP);
		}

		if(actionE != NULL){
			moveTitle.setString("(overwriting)");
			moveTitle.setCharacterSize(20);
			moveTitle.setPosition(x+(ATTW-moveTitle.getLocalBounds().width)/2, y+ATTH-40);
			window.draw(moveTitle);
		}
	}

}

void HUD::events(Event& event, Land* land, RenderWindow& window){
	if(selected != NULL && target != NULL && moveBox.getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y)){
		switch(event.type){
			case Event::MouseWheelScrolled:
				if((event.mouseWheelScroll.delta>0||n>0) && (event.mouseWheelScroll.delta<0 || n < (actionE == NULL ? selected->powerN : actionE->n+selected->powerN) )){
					n += event.mouseWheelScroll.delta;
				}
				break;
			case Event::MouseButtonReleased:
				if(event.mouseButton.button == Mouse::Right){
					target = NULL;
					selected = NULL;
				}
				if(event.mouseButton.button == Mouse::Left){
					if(actionE == NULL){
						if(n > 0){
							//selected->attack(target, n);
							selected->powerN -= n; //Power on target tile is not changed to prevent new actions
							action actionN;
							actionN.from = selected;
							actionN.to = target;
							actionN.n = n;
							actions.push_back(actionN);
						}
					}else{ //Ovewrite action
						selected->powerN += actionE->n - n;
						actionE->n = n;
						if(n<1)actions.erase(actions.begin()+actionEPos);
					}

					target = NULL;
					selected = NULL;
				}
				break;
			default:
				break;
		}
	}else if(actsBounds.getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y)){
		switch(event.type){
			case Event::MouseWheelScrolled:
				if( (event.mouseWheelScroll.delta>0 || actsScroll>0) && (event.mouseWheelScroll.delta<0 || actsScroll < max(0, static_cast<int>(actions.size()*14 - (PAVH-20)) )) ){
					actsScroll += 10*event.mouseWheelScroll.delta;
				}
				break;
			case Event::KeyReleased:
				if(event.key.code == Keyboard::Return){ //TODO This should actually use a clickable button
					Land::pending = PUSHACTIONS;
					//land->startAnim();
				}
			default:
				break;
		}
	}
}

void HUD::setTiles(Tile* t, Tile* s){
	target = t;
	selected = s;

	for(int i=0; i<actions.size(); i++){
		if(s != NULL && actions[i].from == s && actions[i].to == t){
			actionE = &actions[i];
			actionEPos = i;
			break;
		}
	}

	n = (actionE==NULL ? s->powerN : actionE->n);
}

bool HUD::isBusy(int x, int y, float px, float py, float z){
	return (target!=NULL && moveBox.getGlobalBounds().contains(Vector2f(x,y))) || bar.getGlobalBounds().contains(Vector2f(x,y)); //Could include 'actsBounds'

}
