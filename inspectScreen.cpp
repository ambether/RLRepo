#include "main.hpp"

InspectScreen::InspectScreen(int width, int height) : UiElement(width, height) {}

void InspectScreen::render() {
	con->setDefaultBackground(TCODColor::black);
	con->setDefaultForeground(TCODColor::darkerGrey);
	con->clear();
	con->printFrame(0, 0, width, height);

	// Renders the name of Entities under the mouse cursor
	int trueX = engine.mouse.cx + engine.viewport->getOffsetX();
	int trueY = engine.mouse.cy + engine.viewport->getOffsetY();

	if(engine.dungeon->isInFov(trueX, trueY)) {
		char buf[128] = ""; // A buffer of characters
		bool first = true;
		for(auto & ent : engine.entityList) {
			if(ent->x == trueX && ent->y == trueY) {
				if(!first) strcat_s(buf, ", "); // If not the first Entity add a comma and a space
				else first = false;
				strcat_s(buf, ent->name);
			}
		}
		con->setDefaultForeground(TCODColor::lightGrey);
		con->print(2, 1, buf);
	}

	TCODConsole::blit(con, 0, 0, width, height,
		TCODConsole::root, engine.screenWidth - width, engine.screenHeight - height);
}
