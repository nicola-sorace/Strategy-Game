#include "Land.h"
int Land::pending = DONOTHING; //Next network flag to broadcast

void printRequest(TcpSocket* socket, string request){
	cout << socket->getRemoteAddress() <<": "<< request << endl;
}

int Land::client(){
	TcpSocket gameSocket;
	TcpSocket* socket = &gameSocket; //Placeholder pointer
	if(socket->connect("localhost", 4242) == Socket::Done){
		string banner;
		Packet packet;

		socket->receive(packet);
		packet >> banner;
		cout << banner << endl;

		packet.clear();
		packet << GRIDUPDATE;
		socket->send(packet);

		int x,y,type,owner,power;

		cout << "Getting map...";
		for(int i=0; i<LW*LH; i++){
			packet.clear();
			socket->receive(packet);
			packet >> x >> y >> type >> owner >> power;
			//cout << to_string(x) << ":" << to_string(y) << ":" << to_string(owner) << ":" << to_string(power) << endl;
			grid[x][y].type = type;
			grid[x][y].owner = owner;
			grid[x][y].power = power;
			grid[x][y].powerN = power;
		}
		cout << "success" << endl;

		int request = 0;
		while(true){
			//SEND REQUEST:
			int tmpPend = pending; //Clone value so it can't be modified by other threads
			packet.clear();
			packet << tmpPend;
			socket->send(packet);
			switch(tmpPend){
				case PUSHACTIONS:
					packet.clear();
					packet << static_cast<int>(hud.actions.size());
					socket->send(packet);
					for(int i=0; i<hud.actions.size(); i++){
						packet.clear();
						packet << hud.actions[i].from->x << hud.actions[i].from->y << hud.actions[i].to->x << hud.actions[i].to->y << hud.actions[i].n;
						socket->send(packet);
					}
					break;
				default:
					break;
			}
			pending = DONOTHING;

			//INTERPRET RESPONSE:
			packet.clear();
			if(socket->receive(packet) != Socket::Status::Done)cout << "Error: Server failed to respond" << endl;
			packet >> request;

			if(socket->getRemoteAddress() != IpAddress::None && request>0){

				switch(request){
					case DONOTHING:
						//printRequest(&socket,"DO NOTHING");
						break;
					case ENDROUND:
						cout << "Server declared end of round." << endl;
						int actLength;
						packet.clear();
						socket->receive(packet);
						packet >> actLength;

						cout << "Attempting to receive "<<to_string(actLength)<<" actions..." << endl;
						hud.actions.clear();
						for(int i=0; i<actLength; i++){
							action actionN;
							int fx, fy; //From tile
							int tx, ty; //To tile
							int power;

							packet.clear();
							socket->receive(packet);
							packet >> fx >> fy >> tx >> ty >> power;

							actionN.from = &grid[fx][fy];
							actionN.to = &grid[tx][ty];
							actionN.n = power;
							hud.actions.push_back(actionN);
						}
						cout << "Received " << to_string(actLength) << " actions." << endl;

						startAnim();

						break;
					default:
						printRequest(socket,"UNRECOGNIZED COMMAND: "+to_string(request));
						break;
				}
				request = 0;
			}else{
				cout << "Lost connection to server" << endl;
				break;
			}
		}

	}

	cout << "ERR" << endl;
	return 0;
}

void Land::startNetwork(Land* land){
	land->client();
}

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

	thread connection(startNetwork, this);
	connection.detach();
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
