#pragma once

class StatScreen : public UiElement {
public:
	StatScreen(int width, int height);
	void render() const override; 
	void renderBar(int x, int y, int width,
		const char * name,
		float value, float maxValue,
		const TCODColor & barColor, const TCODColor & backColor) const;
};