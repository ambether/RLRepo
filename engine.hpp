#pragma once

class Engine {
private:
	struct char_cmp {
		bool operator()(const char * a, const char * b) const {
			return strcmp(a, b) < 0;
		}
	};
public:
	enum gameState { START, IDLE, TURN, NEW_TURN, WIN, LOSE } gameState;

	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	
	typedef std::map<const char *, shared_ptr<Entity>, char_cmp> EntityMap; // The type for the template maps.
	typedef std::map<const char *, shared_ptr<Spell>, char_cmp> SpellMap; // The type for the Spell template map.

	EntityMap entityTemplates; // A map of Entity templates. These templates will be cloned to make new Entities.
	EntityMap itemTemplates; // A map of Item templates. These templates will be cloned to make new Items.
	SpellMap spellTemplates; // A map of Spell templates. These templates will be cloned to make new Spells.

	vector<shared_ptr<Entity>> entityList;		// A list of all Entities in the game.
	vector<shared_ptr<Entity>> activeEntities;	// It is assumed Entities in this list have Ai, Mortal, and are not dead.
	vector<shared_ptr<Entity>> inactiveEntities;	// It is assumed Entities in this list do not have Ai or Mortal.
	vector<shared_ptr<Entity>> deadEntities;		// Entities with Ai and Mortal, but are dead.

	shared_ptr<Entity> player;

	ActionQueue actionQueue;
	
	int fovRadius;
	int screenWidth, screenHeight;
	shared_ptr<Map> dungeon;
	shared_ptr<Ui> ui;

	Engine(int sW, int sH);
	~Engine();

	shared_ptr<Entity> getClosestMonster(int x, int y, float range) const;
	shared_ptr<Entity> getMonster(int x, int y) const;

	bool pickTile(int * x, int * y, float maxRange = 0.0f, float radius = 0.0f);

	void update();
	void render();
	void notifyDeath(shared_ptr<Entity> entity);
	void addAction(shared_ptr<Action> action);
	void setComputeFov(bool value);
private:
	bool computeFov;
	vector<shared_ptr<Entity>> deaths;

	static float getDistance(int x1, int y1, int x2, int y2);

	void handleDeadEntities();
};

extern Engine engine;