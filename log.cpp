#include "main.hpp"

Log::Log(int width, int height) : UiElement(width, height) {}

Log::~Log() { log.clear(); }

void Log::render() {
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
	TCODConsole::blit(con, 0, 0, width, height,
		TCODConsole::root, 0, engine.screenHeight - height);
}

void Log::message(const TCODColor & col, char text[128]) {
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