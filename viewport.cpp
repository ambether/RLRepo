#include "main.hpp"

Viewport::Viewport(int mapWidth, int mapHeight, int viewWidth, int viewHeight)
	: viewportX(0), viewportY(0), viewWidth(viewWidth), viewHeight(viewHeight) {
	mapConsole = new TCODConsole(mapWidth, mapHeight); 
}

Viewport::~Viewport() { delete mapConsole; }

void Viewport::render() {
	mapConsole->clear();
	///////TODO

}
