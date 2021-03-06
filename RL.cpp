// RL.cpp : Defines the entry point for the console application.
//

#include "main.hpp"

Engine engine(100, 60);

int main() {
	while(!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
    return 0;
}

