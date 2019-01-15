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