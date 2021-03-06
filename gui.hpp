#pragma once

class Gui {
public:
	static const int height = 7;

	Gui();
	~Gui();

	void render();
	void message(const TCODColor & col, const char * text, ...);
protected:
	TCODConsole * con;
	struct Message;
	std::vector<std::shared_ptr<Message>> log;

	void renderBar(int x, int y, int width, 
		const char * name,
		float value, float maxValue, 
		const TCODColor & barColor, const TCODColor & backColor);
	void renderMouseLook();
private:
	static const int bWidth = 20,
		msgX = bWidth + 2,
		msgHeight = height - 1;
};

struct Gui::Message {
	char * text;
	TCODColor col;

	Message(const char * text, const TCODColor & col);
	~Message();
};