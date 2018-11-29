#include "main.hpp"

ActionQueue::ActionQueue() {}

void ActionQueue::addAction(std::shared_ptr<Action> action) { queue.push(action); }

void ActionQueue::processActions() {
	while(!queue.empty()) {
		std::shared_ptr<Action> act = queue.front();
		queue.pop();
		Action::ActionResult result = act->execute();
		while(result.alternate != NULL) {
			act = result.alternate;
			result = act->execute();
		}
	}
}