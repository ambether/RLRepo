#include "main.hpp"

Viewport::Viewport(int width, int height, int mapWidth, int mapHeight)
	: UiElement(width, height), 
	viewportX(0), viewportY(0),
	//viewWidth(viewWidth), viewHeight(viewHeight),
	mapWidth(mapWidth), mapHeight(mapHeight),
	halfViewWidth(width/2), halfviewHeight(height/2),
	diffWidth(mapWidth-width), diffHeight(mapHeight-height) {
	mapConsole = new TCODConsole(mapWidth, mapHeight);
}

Viewport::~Viewport() { delete mapConsole; }

void Viewport::render() const { TCODConsole::blit(mapConsole, viewportX, viewportY, width, height, TCODConsole::root, 0, 0); }

void Viewport::renderMap(shared_ptr<Map> map) { map->render(mapConsole); }

void Viewport::renderEntity(shared_ptr<Entity> entity) { entity->render(mapConsole); }

void Viewport::clear() { mapConsole->clear(); }

void Viewport::moveView(shared_ptr<Entity> & centeredOn) { calculateView(centeredOn); }

int Viewport::getOffsetX() const { return viewportX; }

int Viewport::getOffsetY() const { return viewportY; }

int Viewport::getWidth() const { return width; }

int Viewport::getHeight() const { return height; }

TCODColor Viewport::getCharBackground(int x, int y) const { return mapConsole->getCharBackground(x, y); }

void Viewport::setCharBackground(int x, int y, const TCODColor & col) { mapConsole->setCharBackground(x, y, col); }

void Viewport::calculateView(shared_ptr<Entity> & centeredOn) {
	int tx = centeredOn->x,
		ty = centeredOn->y;
	viewportX = tx - halfViewWidth;
	viewportY = ty - halfviewHeight;

	viewportX = MAX(0, viewportX);
	viewportY = MAX(0, viewportY);

	viewportX = MIN(viewportX, diffWidth);
	viewportY = MIN(viewportY, diffHeight);
}