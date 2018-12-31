#include "main.hpp"

// Ui

Ui::Ui() {
	static const int LOWER_BAR_HEIGHT = 8, // This should be in the header but it won't work for some reason?
		RIGHT_BAR_WIDTH = 20;
	inspectScreen = std::make_shared<InspectScreen>(RIGHT_BAR_WIDTH, LOWER_BAR_HEIGHT);
	log = std::make_shared<Log>(engine.screenWidth - RIGHT_BAR_WIDTH, LOWER_BAR_HEIGHT);
	statScreen = std::make_shared<StatScreen>(RIGHT_BAR_WIDTH, engine.screenHeight - LOWER_BAR_HEIGHT);
}

void Ui::render() {
	inspectScreen->render();
	log->render();
	statScreen->render();
}

void Ui::message(const TCODColor & col, const char * text, ...) { 
	va_list ap;
	char buf[128];
	va_start(ap, text);
	vsprintf_s(buf, text, ap);
	va_end(ap);
	log->message(col, buf);
}


// _uiElement
/*
Ui::_uiElement::_uiElement(int width, int height) : width(width), height(height) { con = new TCODConsole(width, height); }

Ui::_uiElement::~_uiElement() { delete con; }

void Ui::_uiElement::renderBar(int x, int y, int width,
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


// Log

Ui::Log::Log(int width, int height) : _uiElement(width, height) {}

Ui::Log::~Log() { log.clear(); }

void Ui::Log::render() { //// Modify viewport to only render up to the log and the stat screen
	con->setDefaultBackground(TCODColor::black);
	con->setDefaultForeground(TCODColor::darkerGrey);
	con->clear();
	con->printFrame(0, 0, width, height);
	// y will be responsible for the message position on the console
	// cCoef is short for "color coefficient" and will make text look darker
	int y = 1;
	float cCoef = 0.4f;

	for(auto & msg : log) {
		// Multiply the foreground color by the color coefficient, then print the text to the console at position msgX, y
		con->setDefaultForeground(msg->col * cCoef);
		con->print(msgX, y, msg->text);
		++y;

		// Test to see if the color coeff is less than 1.0f; If it is add 0.3f to it
		if(cCoef < 1.0f) { cCoef += 0.3f; }
	}
	TCODConsole::blit(con, 0, 0, engine.screenWidth, height,
		TCODConsole::root, 0, engine.screenHeight - height);
}

void Ui::Log::message(const TCODColor & col, char text[128]) {
	char * lineBegin = text;
	char * lineEnd;
	do {
		if(size(log) == height - 1) { log.erase(begin(log)); }
		lineEnd = strchr(lineBegin, '\n');
		if(lineEnd) { *lineEnd = '\0'; }
		std::shared_ptr<Message> msg = std::make_shared<Message>(lineBegin, col);
		log.push_back(msg);
		lineBegin = lineEnd + 1;
	} while(lineEnd);
}
*/