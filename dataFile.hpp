#pragma once

class DataFile {

public:
	DataFile();

	void parseEntities();
	void parseItems();
	void parseSpells();

	shared_ptr<TCODParser> parser;

	vector<shared_ptr<Entity>> entList;
	vector<shared_ptr<Entity>> itemList; // Entities with the Loot component
	vector<shared_ptr<Spell>> spellList;
private:
	class EntityParserListener;
	class ItemParserListener;
	class SpellParserListener;
};

// EntityParserListener

class DataFile::EntityParserListener : public ITCODParserListener {
public:
	EntityParserListener(vector<shared_ptr<Entity>> * entList);
	bool parserNewStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name);
	bool parserFlag(TCODParser * parser, const char * name);
	bool parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value);
	bool parserEndStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name);
	void error(const char * msg);
private:
	shared_ptr<Entity> currentEntity;
	vector<shared_ptr<Entity>> * entList;
};


// ItemParserListener

class DataFile::ItemParserListener : public ITCODParserListener {
public:
	ItemParserListener(vector<shared_ptr<Entity>> * itemList);
	~ItemParserListener();
	bool parserNewStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name);
	bool parserFlag(TCODParser * parser, const char * name);
	bool parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value);
	bool parserEndStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name);
	void error(const char * msg);
private:
	shared_ptr<Entity> currentItem;
	vector<shared_ptr<Entity>> * itemList;

	struct _HealerData;
	struct _DamageSpellItemData;

	_HealerData * healerData;
	_DamageSpellItemData * damageSpellData;
};

struct DataFile::ItemParserListener::_HealerData {
	int amt;
	_HealerData() : amt(0) {}
	~_HealerData() {}
};

struct DataFile::ItemParserListener::_DamageSpellItemData {
	float range, radius;
	int dmg;
	_DamageSpellItemData() : range(0.0f), dmg(0), radius(0.0f) {}
	~_DamageSpellItemData() {}
};


// SpellParserListener

class DataFile::SpellParserListener : public ITCODParserListener {
public:
	SpellParserListener(vector<shared_ptr<Spell>> * spellList);
	bool parserNewStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name);
	bool parserFlag(TCODParser * parser, const char * name);
	bool parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value);
	bool parserEndStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name);
	void error(const char * msg);
private:
	shared_ptr<Spell> currentSpell;
	vector<shared_ptr<Spell>> * spellList;
};