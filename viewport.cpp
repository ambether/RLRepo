#include "main.hpp"

Viewport::Viewport(int mapWidth, int mapHeight, int viewWidth, int viewHeight)
	: viewportX(0), viewportY(0),
	viewWidth(viewWidth), viewHeight(viewHeight),
	mapWidth(mapWidth), mapHeight(mapHeight),
	halfViewWidth(viewWidth/2), halfviewHeight(viewHeight/2),
	diffWidth(mapWidth-viewWidth), diffHeight(mapHeight-viewHeight) {
	mapConsole = new TCODConsole(mapWidth, mapHeight);
}

Viewport::~Viewport() { delete mapConsole; }

void Viewport::render() const { TCODConsole::blit(mapConsole, viewportX, viewportY, viewWidth, viewHeight, TCODConsole::root, 0, 0); }

void Viewport::clear() { mapConsole->clear(); }

void Viewport::moveView(std::shared_ptr<Entity> & centeredOn) { calculateView(centeredOn); }

int Viewport::getOffsetX() const { return viewportX; }

int Viewport::getOffsetY() const { return viewportY; }

void Viewport::calculateView(std::shared_ptr<Entity> & centeredOn) {
	int tx = centeredOn->x,
		ty = centeredOn->y;
	viewportX = tx - halfViewWidth;
	viewportY = ty - halfviewHeight;

	viewportX = MAX(0, viewportX);
	viewportY = MAX(0, viewportY);

	viewportX = MIN(viewportX, diffWidth);
	viewportY = MIN(viewportY, diffHeight);
}