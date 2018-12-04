#pragma once

struct Tile {
	bool canWalk;
	bool isExplored;

	Tile() : canWalk(false), isExplored(false) {}
};

class Map {
public:
	int w, h;

	Map(int w, int h);
	~Map();

	bool canWalk(int x, int y) const;
	bool isWall(int x, int y) const;
	bool isExplored(int x, int y) const;
	bool isInFov(int x, int y) const;

	void computeFov();
	void render(TCODConsole * mapConsole) const;
	void addMonster(int x, int y);

protected:
	Tile * tiles;
	std::shared_ptr<TCODMap> map;
	//std::unique_ptr<TCODConsole> mapConsole;

	friend class bspList;
	
	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2);
	void addItem(int x, int y);
};