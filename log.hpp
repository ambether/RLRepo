#pragma once

class Log : public UiElement {
public:
	Log(int width, int height);
	~Log();
	void render() const override;
	void message(const TCODColor & col, char text[128]);
private:
	struct Message {
		const char * text;
		TCODColor col;
		Message(const char * text, const TCODColor & col) : text(_strdup(text)), col(col) {}
		~Message() { delete text; }
	};
	vector<shared_ptr<Message>> log;
	static const int msgX = 2;
};