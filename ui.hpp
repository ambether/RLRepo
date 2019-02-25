#pragma once

class Ui {
public:
	Ui();
	void render() const;
	void message(const TCODColor & col, const char * text, ...);
	
	shared_ptr<Viewport> viewport;
private:
	shared_ptr<InspectScreen> inspectScreen;
	shared_ptr<Log> log;
	shared_ptr<StatScreen> statScreen;
};