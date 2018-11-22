#pragma once

class Engine {
public:
	enum gameState { START, IDLE, TURN, WIN, LOSE } gameState;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	//std::vector<std::shared_ptr<Entity>> entityList;
	std::deque<std::shared_ptr<Entity>> entityList;
	std::shared_ptr<Entity> player;
	
	int fovRadius;
	int screenWidth, screenHeight;
	std::unique_ptr<Map> dungeon;
	Gui * gui;

	Engine(int sW, int sH);
	~Engine();

	std::shared_ptr<Entity> getClosestMonster(int x, int y, float range) const;
	std::shared_ptr<Entity> getMonster(int x, int y) const;

	bool pickTile(int * x, int * y, float maxRange = 0.0f);

	void update();
	void render();
	void notifyDeath(std::shared_ptr<Entity> entity);

private:
	bool computeFov;
	std::vector<std::shared_ptr<Entity>> deadEntities;

	void handleDeadEntities();
};

extern Engine engine;