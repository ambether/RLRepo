#include "main.hpp"

InspectScreen::InspectScreen(int width, int height) : UiElement(width, height) {}

void InspectScreen::render() const {
	con->setDefaultBackground(TCODColor::black);
	con->setDefaultForeground(TCODColor::darkerGrey);
	con->clear();
	con->printFrame(0, 0, width, height);

	// Renders the name of Entities under the mouse cursor
	int trueX = engine.mouse.cx + engine.ui->viewport->getOffsetX();
	int trueY = engine.mouse.cy + engine.ui->viewport->getOffsetY();

	if(engine.dungeon->isInFov(trueX, trueY)) {
		char buf[128] = ""; // A buffer of characters
		bool first = true;
		int lineCount = 0;
		for(auto & ent : engine.entityList) {
			if(ent->x == trueX && ent->y == trueY) {
				++lineCount;
				if(!first) strcat_s(buf, "\n"); // If not the first Entity put it on a new line
				else first = false;
				if(lineCount == height - 2) { // If the list would spill over cut it short
					strcat_s(buf, "...");
					break;
				}
				else strcat_s(buf, ent->name);
			}
		}
		con->setDefaultForeground(TCODColor::lightGrey);
		con->print(2, 1, buf);
	}

	TCODConsole::blit(con, 0, 0, width, height,
		TCODConsole::root, engine.screenWidth - width, engine.screenHeight - height);
}
