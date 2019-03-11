#pragma once

class ActionQueue {
public:
	ActionQueue();

	void addAction(shared_ptr<Action> action);
	void processActions();

private:
	std::queue<shared_ptr<Action>> queue;
};