#include "main.hpp"

Gui::Gui() { con = new TCODConsole(engine.screenWidth, height); }

Gui::~Gui() {
	delete con;
	log.clear();
}

void Gui::render() {
	con->setDefaultBackground(TCODColor::black);
	con->clear();

	renderBar(1, 1, bWidth, "HP", engine.player->mortal->hp, engine.player->mortal->maxHp, TCODColor::lightRed, TCODColor::darkerRed);

	// y will be responsible for our message position on the console
	// cCoef is short for "color coefficient" and will make text look darker
	int y = 1;
	float cCoef(0.4f);

	for(auto & msg : log) {
		// Multiply the foreground color by the color coefficient, then print the text to the console at position msgX, y
		con->setDefaultForeground(msg->col * cCoef);
		con->print(msgX, y, msg->text);
		++y;

		// Test to see if the color coeff is less than 1.0f; If it is add 0.3f to it
		if(cCoef < 1.0f) { cCoef += 0.3f; }
	}

	// Render mouse look
	renderMouseLook();

	// Blit the console to the root console
	TCODConsole::blit(con, 0, 0, engine.screenWidth, height, 
		TCODConsole::root, 0, engine.screenHeight - height);
}

void Gui::message(const TCODColor & col, const char * text, ...) {
	va_list ap;
	char buf[128];
	va_start(ap, text);
	vsprintf_s(buf, text, ap);
	va_end(ap);

	char * lineBegin = buf;
	char * lineEnd;
	do {
		if(size(log) == msgHeight) { log.erase(begin(log)); }
		lineEnd = strchr(lineBegin, '\n');
		if(lineEnd) { *lineEnd = '\0'; }
		std::shared_ptr<Message> msg = std::make_shared<Message>(lineBegin, col);
		log.push_back(msg);
		lineBegin = lineEnd + 1;
	} while(lineEnd);
}

void Gui::renderBar(int x, int y, int width,
	const char * name, 
	float value, float maxValue, 
	const TCODColor & barColor, const TCODColor & backColor) {

	con->setDefaultBackground(backColor);
	con->rect(x, y, width, 1, false, TCOD_BKGND_SET);

	int barWidth = (int)(value / maxValue * width);
	if(barWidth > 0) {
		con->setDefaultBackground(barColor);
		con->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	con->setDefaultForeground(TCODColor::white);
	con->printEx(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s : %g/%g", name, value, maxValue);
}

Gui::Message::Message(const char * text, const TCODColor & col) : text(_strdup(text)), col(col) {}

Gui::Message::~Message() { delete text; }

void Gui::renderMouseLook() {
	int trueX = engine.mouse.cx + engine.viewport->getOffsetX();
	int trueY = engine.mouse.cy + engine.viewport->getOffsetY();

	if(!engine.dungeon->isInFov(trueX, trueY)) { return; }
	char buf[128] = ""; // A buffer of characters
	bool first = true;

	for(auto & ent : engine.entityList) {
		if(ent->x == trueX && ent->y == trueY) {
			if(!first) { strcat_s(buf, ", "); } // If not the first, add a comma and a space.
			else { first = false; }
			strcat_s(buf, ent->name);
		}
	}
	con->setDefaultForeground(TCODColor::lightGrey);
	con->print(1, 0, buf);
}

