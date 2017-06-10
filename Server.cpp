#include <SFML/Network.hpp>
#include <iostream>
#include <sstream>
#include <regex>
#include <thread>
using namespace sf;
using namespace std;

#define MAXCONNECTIONS 2 //Number of players per game
TcpSocket clients[MAXCONNECTIONS];

#define N MAXCONNECTIONS+1 //Number of players in game + 1

#define DONOTHING 1
#define GRIDUPDATE 2
#define PUSHACTIONS 3 //Client request to add actions to server
#define ENDROUND 4 //Broadcast round over and list of actions

int pending[MAXCONNECTIONS]; //Next update to send

struct Tile{
	int x,y;

	int type;
	int owner;
	int power;
};

#define LW 25 //Land width and height
#define LH 16
Tile grid[LW][LH];

struct action{
	Tile* from; //NULL if power being created
	Tile* to;
	int n;

	int client; //Stores originating client number (NOTE Client numbers are not static so this is not trustworthy)
};
vector<action> actions;

bool ready[MAXCONNECTIONS]; //Stores whether each player has sent their moves this round.

void printRequest(TcpSocket* socket, int n, string request){
	cout << '\r' << socket->getRemoteAddress() <<" ["<<to_string(n)<<"]: "<< request;
	cout << endl << " > ";
	fflush(stdout);
}

void commandListener(){
	string command;

	while(true){
		getline(cin, command);
		string ars[5]; //command arguments
		int i = 0;
		stringstream ssin(command);
		while(ssin.good() && i<5){
			ssin >> ars[i];
			i++;
		}

		regex eExit("^exit$");
		regex eList("^list$");
		regex eActs("^acts$");
		regex eSend("^send$");
		regex eKick("^kick$");
		regex eHelp("^help$");

		int a1 = -1;
		int a2 = -1;
		try{
			a1 = stoi(ars[1]);
		}catch(invalid_argument& e){

		}catch (out_of_range& e){

		}
		try{
			a2 = stoi(ars[2]);
		}catch(invalid_argument& e){

		}catch (out_of_range& e){

		}

		if(regex_match(ars[0],eExit)){		//EXIT
			cout << "Shutting down server and ending CLI..." << endl;
			exit(0);
		}else if(regex_match(ars[0],eList)){		//LIST
			cout << "#	IP Address	Status" << endl;
			for(int i=0; i<sizeof(clients)/sizeof(*clients); i++){
				if(clients[i].getRemoteAddress() != IpAddress::None){
					cout << to_string(i) <<"	"<< clients[i].getRemoteAddress() <<"	"<< (ready[i] ? "READY" : "thinking...") << endl;
				}
			}
		}else if(regex_match(ars[0],eActs)){		//ACTS
			cout << "#	Client	Power	From	To" << endl;
			for(int i=0; i<actions.size(); i++){
				if(a1 == -1 || actions[i].from->owner == a1+1){
					cout << to_string(i) <<"	"<< to_string(actions[i].client) <<"	"<< to_string(actions[i].n) <<"	("<<to_string(actions[i].from->x)<<","<<to_string(actions[i].from->y)<<")	("<<to_string(actions[i].to->x)<<","<<to_string(actions[i].to->y)<<")" << endl;
					//cout << to_string(i) << "	" << to_string(actions[i].client) << endl;
				}
			}
		}else if(regex_match(ars[0],eSend)){		//SEND
			while(pending[a1] != DONOTHING){}; //Wait for current pending to clear
			pending[a1] = a2;
		}else if(regex_match(ars[0],eKick)){		//KICK
			if(clients[a1].getRemoteAddress() != IpAddress::None){
				cout << "Kicking client number " << to_string(a1) << " from the server..." << endl;
				clients[a1].disconnect();
			}else{
				cout << "Invalid client number (client is not connected)" << endl;
			}
		}else if(regex_match(ars[0],eHelp)){		//HELP
			cout << endl << " Commands:" << endl
			<< "help - Shows help" << endl
			<< "list - Lists all connected clients" << endl
			<< "acts (int player) - Summarizes all (or a specific player's) actions" << endl
			<< "send [int client] [int command] - Sends command [command] to client number [client]" << endl //TODO Select client
			<< "kick [int client] - Kicks client number [client] from server" << endl
			<< "exit - Shutdown server and exit CLI" << endl
			<< endl; //TODO
		}else if(!ars[0].empty()){
			cout << "Unknown command '" << ars[0] << "'. Use 'help' for a list of valid commands." << endl;
		}
		cout << " > ";
	}

}

int client(int n){
	TcpSocket* socket = &clients[n];

	cout << '\r' << "New connection received from " << socket->getRemoteAddress() <<" ["<<to_string(n)<<"]"<< endl << " > ";

	Packet packet;
	packet << "StrategyGame-PreAlpha";
	socket->send(packet);

	int request = 0;

	while(true){
		//INTERPRET REQUEST:
		packet.clear();
		socket->receive(packet);
		packet >> request;

		if(socket->getRemoteAddress() != IpAddress::None && request>0){

			switch(request){
				case DONOTHING:
					//printRequest(socket,n,"DO NOTHING");
					break;
				case GRIDUPDATE:
					printRequest(socket,n,"Updating client grid...");
					for(int x=0; x < LW; x++){
						for(int y=0; y < LH; y++){
							packet.clear();
							packet << x << y << grid[x][y].type << grid[x][y].owner << grid[x][y].power;
							socket->send(packet);
						}
					}
					printRequest(socket,n,"Client grid is up to date.");
					break;
				case PUSHACTIONS:
					int actLength;
					packet.clear();
					socket->receive(packet);
					packet >> actLength;

					printRequest(socket,n,"Receiving "+to_string(actLength)+" actions from client...");
					//TODO Check allowed move

					for(int i=0; i<actLength; i++){
						action actionN;
						int fx, fy; //From tile
						int tx, ty; //To tile
						int power;

						packet.clear();
						socket->receive(packet);
						packet >> fx >> fy >> tx >> ty >> power;

						actionN.client = n;
						actionN.from = &grid[fx][fy];
						actionN.to = &grid[tx][ty];
						actionN.n = power;
						actions.push_back(actionN);
					}
					ready[n] = true;
					printRequest(socket,n,"Client actions received.");

					break;
				default:
					printRequest(socket,n,"UNRECOGNIZED COMMAND: "+to_string(request));
					break;
			}
			request = 0;

			//SEND UPDATES:

			bool roundOver = true;
			for(int i=0; i<sizeof(ready)/sizeof(ready[0]); i++)if(!ready[i]){
				roundOver = false;
				break;
			}

			packet.clear();
			if(roundOver){ //TODO Condition to end round
				//TODO Reorder actions?
				packet << ENDROUND;
				socket->send(packet);
				packet.clear();
				packet << static_cast<int>(actions.size());
				socket->send(packet);

				for(int i=0; i<actions.size(); i++){
					packet.clear();
					packet << actions[i].from->x << actions[i].from->y << actions[i].to->x << actions[i].to->y << actions[i].n;
					socket->send(packet);
				}
				//TODO Clear actions and update grid once all clients have ended round
				actions.clear();
				for(int i=0; i<sizeof(ready)/sizeof(ready[0]); i++)ready[i]=false;

				pending[n] = DONOTHING; //Any pending actions are cancelled since round is over
			}else{
				int tmpPend = pending[n]; //Clone value so it can't be modified by other threads
				packet << tmpPend;
				socket->send(packet);
				if(tmpPend != DONOTHING) pending[n] = DONOTHING;
			}

		}else{
			cout << '\r' << "Client" <<" ["<<to_string(n)<<"] has disconnected";
			cout << endl << " > ";
			fflush(stdout);
			break;
		}

	}
}

int waitForConnections(TcpListener* listener){
	cout << "Generating map..." << endl;
	for(int x=0; x < LW; x++){
		for(int y=0; y < LH; y++){
			grid[x][y].x = x;
			grid[x][y].y = y;
			grid[x][y].type = rand()%2;
			grid[x][y].owner = rand()%N;
			grid[x][y].power = grid[x][y].owner&&grid[x][y].type==0 ? rand()%8 : 0;
		}
	}

	cout << "Server started on port " << listener->getLocalPort() << endl;

	cout << " > ";
	thread cli(commandListener);

	while(true){
		for(int i=0; i<(sizeof(clients)/sizeof(*clients)); i++){
			if(clients[i].getRemoteAddress() == IpAddress::None){
				TcpSocket* socket = &clients[i];

				if(listener->accept(*socket) == Socket::Done){
					thread c(client, i);
					c.detach();
				}
				break;
			}
		}
	}

}

int main(){
	srand (time(NULL));

	for(int i=0; i<MAXCONNECTIONS; i++)pending[i] = DONOTHING;

	TcpListener listener;
	listener.listen(4242);

	thread doorman(waitForConnections, &listener);

	doorman.join();

	return 0;
}
