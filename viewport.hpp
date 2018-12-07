#pragma once

class Viewport {
public:
	Viewport(int mapWidth, int mapHeight, int viewWidth, int viewHeight);
	~Viewport();
	
	void render() const;
	void renderMap(std::shared_ptr<Map> map);
	void renderEntity(std::shared_ptr<Entity> entity);
	void clear();
	void moveView(std::shared_ptr<Entity> & centeredOn);

	int getOffsetX() const;
	int getOffsetY() const;
	int getViewWidth() const;
	int getViewHeight() const;

	TCODColor getCharBackground(int x, int y) const;
	void setCharBackground(int x, int y, const TCODColor & col);
private:
	TCODConsole * mapConsole;

	int viewportX, viewportY,
		mapWidth, mapHeight,
		viewWidth, viewHeight;

	int const halfViewWidth, halfviewHeight,
		diffWidth, diffHeight;

	void calculateView(std::shared_ptr<Entity> & centeredOn);
};