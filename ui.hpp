#pragma once

class Ui {
public:
	Ui();
	void render() const;
	void message(const TCODColor & col, const char * text, ...);
	
	std::shared_ptr<Viewport> viewport;
private:
	std::shared_ptr<InspectScreen> inspectScreen;
	std::shared_ptr<Log> log;
	std::shared_ptr<StatScreen> statScreen;
};

/*
class Ui::_uiElement {
public:
	_uiElement(int width, int height);
	~_uiElement();
	virtual void render() = 0;
	void renderBar(int x, int y, int width,
		const char * name, 
		float value, float maxValue,
		const TCODColor & barColor, const TCODColor & backColor);
protected:
	int x, y, width, height;
	TCODConsole * con;
};


class Ui::StatScreen : public Ui::_uiElement {

};


class Ui::Log : public Ui::_uiElement {
public:
	Log(int width, int height);
	~Log();
	void render() override;
	void message(const TCODColor & col, char text [128]);
private:
	struct Message {
		char * text;
		TCODColor col;

		Message(const char * text, const TCODColor & col) : text(_strdup(text)), col(col) {}
		~Message() { delete text; }
	};
	std::vector<std::shared_ptr<Message>> log;
	static const int msgX = 2;
};


class Ui::InspectScreen {

};
*/