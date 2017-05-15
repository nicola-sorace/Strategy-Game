struct action{
	Tile* from; //NULL if power being created
	Tile* to;
	int n;
};

class HUD : public Interface{
	public:
		HUD();
		void draw(RenderWindow&, Font*, Color[N], string[N], int player, Tile* s, float px, float py, float z);
		void events(Event&, RenderWindow& window);
		void setTiles(Tile* t, Tile* s);
		bool isBusy(int x, int y, float px, float py, float z); //Returns true if point on screen is busy taking HUD input
		Tile* selected;
		Tile* target;
		vector<action> actions;

	private:
		RectangleShape bar;

		action* actionE; //Existing action, if existing (for overwriting)
		int actionEPos; //Location of existing action within actions array
		int n; //Power to be transferred
		RectangleShape moveBox;

		int actsScroll; //Scroll position of view
		RectangleShape actsBounds;
};
