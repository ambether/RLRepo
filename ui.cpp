#include "main.hpp"

Ui::Ui() {
	static const int BOTTOM_BAR_HEIGHT = 12, // This should be in the header but it won't work for some reason?
		RIGHT_BAR_WIDTH = 25;
	viewport = std::make_shared<Viewport>(
		engine.screenWidth - RIGHT_BAR_WIDTH, 
		engine.screenHeight - BOTTOM_BAR_HEIGHT, 
		engine.dungeon->getWidth(),
		engine.dungeon->getHeight());
	inspectScreen = std::make_shared<InspectScreen>(RIGHT_BAR_WIDTH, BOTTOM_BAR_HEIGHT);
	log = std::make_shared<Log>(engine.screenWidth - RIGHT_BAR_WIDTH, BOTTOM_BAR_HEIGHT);
	statScreen = std::make_shared<StatScreen>(RIGHT_BAR_WIDTH, engine.screenHeight - BOTTOM_BAR_HEIGHT);
}

void Ui::render() const {
	viewport->render();
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