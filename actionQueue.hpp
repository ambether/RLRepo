#pragma once

class ActionQueue {
public:
	ActionQueue();

	void addAction(std::shared_ptr<Action> action);
	void processActions();

private:
	std::queue<std::shared_ptr<Action>> queue;
};