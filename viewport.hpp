#pragma once

class Viewport : public UiElement {
public:
	Viewport(int width, int height, int mapWidth, int mapHeight);
	~Viewport();
	
	void render() const override;
	void renderMap(std::shared_ptr<Map> map);
	void renderEntity(std::shared_ptr<Entity> entity);
	void clear();
	void moveView(std::shared_ptr<Entity> & centeredOn);

	int getOffsetX() const;
	int getOffsetY() const;
	int getWidth() const;
	int getHeight() const;

	TCODColor getCharBackground(int x, int y) const;
	void setCharBackground(int x, int y, const TCODColor & col);
private:
	TCODConsole * mapConsole;

	int viewportX, viewportY,
		mapWidth, mapHeight;

	int const halfViewWidth, halfviewHeight,
		diffWidth, diffHeight;

	void calculateView(std::shared_ptr<Entity> & centeredOn);
};