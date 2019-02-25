#pragma once

class DataFile {

public:
	DataFile();

	void parseEntities();

	shared_ptr<TCODParser> parser;
private:
	vector<shared_ptr<Entity>> entList;
	shared_ptr<vector<shared_ptr<Entity>>> itemList; // Entities with the Loot component
	
	class EntityParserListener;
	class ItemParserListener;
};

class DataFile::EntityParserListener : public ITCODParserListener {
public:
	EntityParserListener(vector<shared_ptr<Entity>> * entList);
	bool parserNewStruct(TCODParser * parser, const TCODParserStruct * str, const char * name);
	bool parserFlag(TCODParser * parser, const char * name);
	bool parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value);
	bool parserEndStruct(TCODParser * parser, const TCODParserStruct * str, const char * name);
	void error(const char * msg);
private:
	shared_ptr<Entity> currentEntity;
	vector<shared_ptr<Entity>> * entList;
};

class DataFile::ItemParserListener : public ITCODParserListener {
public:
	ItemParserListener(vector<shared_ptr<Entity>> * itemList);
	bool parserNewStruct(TCODParser * parser, const TCODParserStruct * str, const char * name);
	bool parserFlag(TCODParser * parser, const char * name);
	bool parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value);
	bool parserEndStruct(TCODParser * parser, const TCODParserStruct * str, const char * name);
	void error(const char * msg);
private:
	shared_ptr<Entity> currentItem;
	vector<shared_ptr<Entity>> * itemList;
};