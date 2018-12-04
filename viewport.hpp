#pragma once

class Viewport {
public:
	TCODConsole * mapConsole;

	Viewport(int mapWidth, int mapHeight, int viewWidth, int viewHeight);
	~Viewport();
	
	void render();
private:
	int viewportX, viewportY,
	viewWidth, viewHeight;
};