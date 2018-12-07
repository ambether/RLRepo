#pragma once

class Engine {
public:
	enum gameState { START, IDLE, TURN, NEW_TURN, WIN, LOSE } gameState;

	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	std::vector<std::shared_ptr<Entity>> entityList;		// A list of all Entities in the game.
	std::vector<std::shared_ptr<Entity>> activeEntities;	// It is assumed Entities in this list have Ai, Mortal, and are not dead.
	std::vector<std::shared_ptr<Entity>> inactiveEntities;	// It is assumed Entities in this list do not have Ai or Mortal.
	std::vector<std::shared_ptr<Entity>> deadEntities;		// Entities with Ai and Mortal, but are dead.

	std::shared_ptr<Entity> player;

	ActionQueue actionQueue;
	
	int fovRadius;
	int screenWidth, screenHeight;
	std::shared_ptr<Map> dungeon;
	std::shared_ptr<Viewport> viewport;
	std::shared_ptr<Gui> gui;

	Engine(int sW, int sH);
	~Engine();

	std::shared_ptr<Entity> getClosestMonster(int x, int y, float range) const;
	std::shared_ptr<Entity> getMonster(int x, int y) const;

	bool pickTile(int * x, int * y, float maxRange = 0.0f, float radius = 0.0f);

	void update();
	void render();
	void notifyDeath(std::shared_ptr<Entity> entity);
	void addAction(std::shared_ptr<Action> action);
	void setComputeFov(bool value);
private:
	bool computeFov;
	std::vector<std::shared_ptr<Entity>> deaths;

	static float getDistance(int x1, int y1, int x2, int y2);

	void handleDeadEntities();
};

extern Engine engine;