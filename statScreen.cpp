#include "main.hpp"

StatScreen::StatScreen(int width, int height) : UiElement(width, height) {}

void StatScreen::render() const {
	static const int barWidth = width - 4;
	con->setDefaultBackground(TCODColor::black);
	con->setDefaultForeground(TCODColor::darkerGrey);
	con->clear();
	con->printFrame(0, 0, width, height);

	renderBar(2, 2, barWidth, "HP", engine.player->mortal->hp, engine.player->mortal->maxHp, TCODColor::lightRed, TCODColor::darkerRed);

	con->setDefaultBackground(TCODColor::black);
	con->setDefaultForeground(TCODColor::white);

	con->printFrame(1, 4, width - 2, 6, true, TCOD_BKGND_DEFAULT, "Spell Charges");

	con->print(2, 6, "I:   %d", engine.player->spellCaster->getCharges(1));
	con->print(2, 7, "II:  %d", engine.player->spellCaster->getCharges(2));
	con->print(2, 8, "III: %d", engine.player->spellCaster->getCharges(3));

	TCODConsole::blit(con, 0, 0, width, height,
		TCODConsole::root, engine.screenWidth - width, 0);
}

void StatScreen::renderBar(int x, int y, int width,
	const char * name,
	float value, float maxValue,
	const TCODColor & barColor, const TCODColor & backColor) const {
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