#include "main.hpp"

DataFile::DataFile() {
	entList = std::make_shared<vector<shared_ptr<Entity>>>();
	parser = std::make_shared<TCODParser>();
}

void DataFile::parseEntities() {
	TCODParserStruct * entityTypeStruct = parser->newStructure("Entity"); // Init Entity structure
	entityTypeStruct->addProperty("symbol", TCOD_TYPE_CHAR, true);
	entityTypeStruct->addProperty("col", TCOD_TYPE_COLOR, true);
	entityTypeStruct->addProperty("blocks", TCOD_TYPE_BOOL, false);

	TCODParserStruct * aiTypeStruct = parser->newStructure("Ai"); // Init Ai structure
	aiTypeStruct->addProperty("speed", TCOD_TYPE_INT, false);
	entityTypeStruct->addStructure(aiTypeStruct); // Add Ai as a substructure to Entity

	TCODParserStruct * combatTypeStruct = parser->newStructure("Combat"); // Init Combat structure
	combatTypeStruct->addProperty("dex", TCOD_TYPE_INT, true);
	combatTypeStruct->addProperty("skl", TCOD_TYPE_INT, true);
	combatTypeStruct->addProperty("str", TCOD_TYPE_INT, true);
	entityTypeStruct->addStructure(combatTypeStruct); // Add Combat as a substructure to Entity

	TCODParserStruct * mortalTypeStruct = parser->newStructure("Mortal"); // Init Mortal structure
	mortalTypeStruct->addProperty("hp", TCOD_TYPE_INT, true);
	mortalTypeStruct->addProperty("corpseName", TCOD_TYPE_STRING, true);
	entityTypeStruct->addStructure(mortalTypeStruct); // Add Mortal as a substructure to Entity

	TCODParserStruct * containerTypeStruct = parser->newStructure("Container"); // Init Container structure
	containerTypeStruct->addProperty("size", TCOD_TYPE_INT, true);
	entityTypeStruct->addStructure(containerTypeStruct); // Add Container as a substructure to Entity

	TCODParserStruct * interactionTypeStruct = parser->newStructure("Interaction"); // Init Interaction structure
	entityTypeStruct->addStructure(interactionTypeStruct); // Add Interaction as a substructure to Entity

	TCODParserStruct * spellCasterTypeStruct = parser->newStructure("SpellCaster"); // Init SpellCaster structure
	entityTypeStruct->addStructure(spellCasterTypeStruct); // Add SpellCaster as a substructure to Entity

	parser->run("gamedata.txt", new EntityParserListener(entList)); // Run the parser
	for(auto & e : *entList) {
		printf("inv size: %d\n", e->container ? e->container->size : 0);
	}
}


// ParserListener

DataFile::EntityParserListener::EntityParserListener(shared_ptr<vector<shared_ptr<Entity>>> entList) : entList(entList) {}

bool DataFile::EntityParserListener::parserNewStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name) {
	//engine.ui->message(TCODColor::lightGrey, "New structure, type %s with name %s", strct->getName(), name ? name : "NULL");
	const char * strctName = strct->getName();
	if(strcmp(strctName, "Entity") == 0) {
		currentEntity = std::make_shared<Entity>();
		currentEntity->name = _strdup(name);
		entList->emplace_back(currentEntity);
	}
	else if(strcmp(strctName, "Ai") == 0) {
		if(strcmp(name, "playerAi") == 0)	{ currentEntity->ai = std::make_shared<playerAi>(); }
		else if(strcmp(name, "mobAi") == 0) { currentEntity->ai = std::make_shared<mobAi>();	}
	}
	else if(strcmp(strctName, "Combat") == 0) {
		currentEntity->combat = std::make_shared<Combat>();
	}
	else if(strcmp(strctName, "Mortal") == 0) {
		if(strcmp(name, "player") == 0)		{ currentEntity->mortal = std::make_shared<pcMortal>();  }
		else if(strcmp(name, "npc") == 0)	{ currentEntity->mortal = std::make_shared<npcMortal>(); }
	}
	else if(strcmp(strctName, "Container") == 0) { 
		currentEntity->container = std::make_shared<Container>();
	}
	else if(strcmp(strctName, "Interaction") == 0) {
		if(strcmp(name, "doorInteraction") == 0) {
			currentEntity->interaction = std::make_shared<DoorInteraction>();
		}
	}
	else if(strcmp(strctName, "SpellCaster") == 0) {
		currentEntity->spellCaster = std::make_shared<SpellCaster>();
	}
	return true;
}

bool DataFile::EntityParserListener::parserFlag(TCODParser * parser, const char * name) {
	//engine.ui->message(TCODColor::lightGrey, "found new flag %s", name);
	return true;
}

bool DataFile::EntityParserListener::parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value) {
	//engine.ui->message(TCODColor::lightGrey, "found new property %s", name);
	// Base Entity properties
	if(strcmp(name, "symbol") == 0) { currentEntity->ch = value.c; }
	else if(strcmp(name, "col") == 0) { currentEntity->color = value.col; }
	else if(strcmp(name, "blocks") == 0) { currentEntity->blocks = value.b; }

	// Ai properties
	else if(strcmp(name, "speed") == 0) { if(currentEntity->ai) currentEntity->ai->setSpeed(value.i); }
	
	// Combat properties
	else if(strcmp(name, "dex") == 0) { if(currentEntity->combat) currentEntity->combat->setDex(value.i); }
	else if(strcmp(name, "skl") == 0) { if(currentEntity->combat) currentEntity->combat->setSkl(value.i); }
	else if(strcmp(name, "str") == 0) { if(currentEntity->combat) currentEntity->combat->setStr(value.i); }
	
	// Mortal properties
	else if(strcmp(name, "hp") == 0) { 
		if(currentEntity->mortal) { 
			currentEntity->mortal->maxHp = value.i;
			currentEntity->mortal->hp = value.i;
		} 
	}
	else if(strcmp(name, "corpseName") == 0) { if(currentEntity->mortal) currentEntity->mortal->corpseName = _strdup(value.s); }
	else if(strcmp(name, "size") == 0) { if(currentEntity->container) currentEntity->container->size = value.i; }
	return true;
}

bool DataFile::EntityParserListener::parserEndStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name) {
	//engine.ui->message(TCODColor::lightGrey, "end of structure %s", name);
	return true;
}

void DataFile::EntityParserListener::error(const char * msg) {
	fprintf(stderr, msg);
	exit(1);
}