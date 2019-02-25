#pragma once

class DataFile {

public:
	DataFile();

	void parseEntities();

	shared_ptr<TCODParser> parser;
private:
	shared_ptr<vector<shared_ptr<Entity>>> entList;
	class EntityParserListener;
};

class DataFile::EntityParserListener : public ITCODParserListener {
public:
	EntityParserListener(shared_ptr<vector<shared_ptr<Entity>>> entList);
	bool parserNewStruct(TCODParser * parser, const TCODParserStruct * str, const char * name);
	bool parserFlag(TCODParser * parser, const char * name);
	bool parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value);
	bool parserEndStruct(TCODParser * parser, const TCODParserStruct * str, const char * name);
	void error(const char * msg);
private:
	shared_ptr<Entity> currentEntity;
	shared_ptr<vector<shared_ptr<Entity>>> entList;
};
