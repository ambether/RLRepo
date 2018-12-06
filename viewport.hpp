#pragma once

class Viewport {
public:
	TCODConsole * mapConsole;

	Viewport(int mapWidth, int mapHeight, int viewWidth, int viewHeight);
	~Viewport();
	
	void render() const;
	void clear();
	void moveView(std::shared_ptr<Entity> & centeredOn);

	int getOffsetX() const;
	int getOffsetY() const;

	int viewWidth, viewHeight;
private:
	int viewportX, viewportY,
		mapWidth, mapHeight;

	int const halfViewWidth, halfviewHeight,
		diffWidth, diffHeight;

	void calculateView(std::shared_ptr<Entity> & centeredOn);
};