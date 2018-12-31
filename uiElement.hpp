#pragma once

class UiElement {
public:
	UiElement(int width, int height);
	~UiElement();
	virtual void render() const = 0;
	int getWidth() const;
	int getHeight() const;
protected:
	int width, height;
	TCODConsole * con;
};