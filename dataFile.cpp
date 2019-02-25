#include "main.hpp"

// DataFile

DataFile::DataFile() {
	entList = vector<shared_ptr<Entity>>();
	itemList = vector<shared_ptr<Entity>>();
}

void DataFile::parseEntities() {
	parser = std::make_shared<TCODParser>();

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

	try {
		parser->run("entdata.txt", new EntityParserListener(&entList)); // Run the parser
	}
	catch(std::exception & e) { fprintf(stderr, e.what()); }
}

void DataFile::parseItems() {
	parser = std::make_shared<TCODParser>();

	TCODParserStruct * itemTypeStruct = parser->newStructure("Item"); // Init Item structure
	itemTypeStruct->addProperty("symbol", TCOD_TYPE_CHAR, true);
	itemTypeStruct->addProperty("col", TCOD_TYPE_COLOR, true);

	TCODParserStruct * healerTypeStruct = parser->newStructure("Healer"); // Init Healer structure
	healerTypeStruct->addProperty("amt", TCOD_TYPE_INT, true);
	itemTypeStruct->addStructure(healerTypeStruct); // Add Healer as a substructure to Item

	TCODParserStruct * damageSpellTypeStruct = parser->newStructure("DamageSpell"); // Init Damage Spell Structure
	damageSpellTypeStruct->addProperty("range", TCOD_TYPE_FLOAT, true);
	damageSpellTypeStruct->addProperty("dmg", TCOD_TYPE_INT, true);
	damageSpellTypeStruct->addProperty("radius", TCOD_TYPE_FLOAT, false);
	itemTypeStruct->addStructure(damageSpellTypeStruct); // Add DamageSpell as a substructure to Item
	try {
		parser->run("itemdata.txt", new ItemParserListener(&itemList));
	}
	catch(std::exception & e) { fprintf(stderr, e.what()); }
}


// EntityParserListener

DataFile::EntityParserListener::EntityParserListener(vector<shared_ptr<Entity>> * entList) : entList(entList) {}

bool DataFile::EntityParserListener::parserNewStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name) {
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

bool DataFile::EntityParserListener::parserFlag(TCODParser * parser, const char * name) { return true; }

bool DataFile::EntityParserListener::parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value) {
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

bool DataFile::EntityParserListener::parserEndStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name) { return true; }

void DataFile::EntityParserListener::error(const char * msg) {
	fprintf(stderr, "In EntityParserListener:\n");
	throw(std::exception(msg));
}


// ItemParserListener

DataFile::ItemParserListener::ItemParserListener(vector<shared_ptr<Entity>> * itemList) : itemList(itemList) {}

DataFile::ItemParserListener::~ItemParserListener() {
	delete damageSpellData;
	delete healerData;
}

bool DataFile::ItemParserListener::parserNewStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name) {
	const char * strctName = strct->getName();
	if(strcmp(strctName, "Item") == 0) {
		currentItem = std::make_shared<Entity>();
		currentItem->name = _strdup(name);
		currentItem->blocks = false;
		itemList->emplace_back(currentItem);
	}
	else if(strcmp(strctName, "Healer") == 0) {
		healerData = new _HealerData();
	}
	else if(strcmp(strctName, "DamageSpell") == 0) {
		damageSpellData = new _DamageSpellItemData();
	}
	return true;
}

bool DataFile::ItemParserListener::parserFlag(TCODParser * parser, const char * name) { return true; }

bool DataFile::ItemParserListener::parserProperty(TCODParser * parser, const char * name, TCOD_value_type_t type, TCOD_value_t value) {
	// Base Item properties
	if(strcmp(name, "symbol") == 0) { currentItem->ch = value.c; }
	else if(strcmp(name, "col") == 0) { currentItem->color = value.col; }
	
	// Healer properties
	else if(strcmp(name, "amt") == 0) { healerData->amt = value.i; }
	
	// DamageSpell properties
	else if(strcmp(name, "range") == 0) { damageSpellData->range = value.f; }
	else if(strcmp(name, "dmg") == 0) { damageSpellData->dmg = value.i; }
	else if(strcmp(name, "radius") == 0) { damageSpellData->radius = value.f; }

	return true;
}

bool DataFile::ItemParserListener::parserEndStruct(TCODParser * parser, const TCODParserStruct * strct, const char * name) {
	const char * strctName = strct->getName();
	if(strcmp(strctName, "Healer") == 0 && healerData) {
		currentItem->loot = std::make_shared<Healer>(healerData->amt);
	}
	else if(strcmp(strctName, "DamageSpell") == 0 && damageSpellData) { // Eventually rewrite the part for more generic scrolls
		if(strcmp(currentItem->name, "Lightning Bolt Scroll") == 0) {
			currentItem->loot = std::make_shared<LightningBolt>(damageSpellData->range, damageSpellData->dmg);
		}
		else if(strcmp(currentItem->name, "Fireball Scroll") == 0) {
			currentItem->loot = std::make_shared<Fireball>(damageSpellData->range, damageSpellData->dmg, damageSpellData->radius);
		}
	}
	return true;
}

void DataFile::ItemParserListener::error(const char * msg) {
	fprintf(stderr, "In ItemParserListener:\n");
	throw(std::exception(msg));
}
