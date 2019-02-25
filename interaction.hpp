#pragma once

class Interaction {
public:
	virtual void interact(shared_ptr<Entity> owner, shared_ptr<Entity> interacter) = 0;
	const char * getInteractVerb() const;
protected:
	const char * interactVerb;
};

class DoorInteraction : public Interaction {
public:
	DoorInteraction();
	void interact(shared_ptr<Entity> owner, shared_ptr<Entity> interacter);
private:
	static const int OPEN_CHAR = '\'', CLOSED_CHAR = '+';
	bool open;
};