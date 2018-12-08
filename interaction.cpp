#include "main.hpp"

const char * Interaction::getInteractVerb() const { return interactVerb; }


DoorInteraction::DoorInteraction() : open(false) { interactVerb = "opens"; }

void DoorInteraction::interact(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> interacter) {
	open = !open; // Swaps the state of the door's open-ness
	if(open) { // Opens the door
		owner->blocks = false; 
		owner->ch = OPEN_CHAR;
		interactVerb = "closes";
		engine.dungeon->setTransparent(owner->x, owner->y, true);
	}
	else { // Closes the door
		owner->blocks = true;
		owner->ch = CLOSED_CHAR;
		interactVerb = "opens";
		engine.dungeon->setTransparent(owner->x, owner->y, false);
	}
	engine.setComputeFov(true);
}
