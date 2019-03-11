#pragma once

class Interaction {
public:
	virtual shared_ptr<Interaction> clone() const = 0;
	virtual void interact(shared_ptr<Entity> owner, shared_ptr<Entity> interacter) = 0;
	const char * getInteractVerb() const;
protected:
	const char * interactVerb;
};

class DoorInteraction : public Interaction {
public:
	DoorInteraction();
	DoorInteraction(const DoorInteraction & obj); // Copy ctor
	shared_ptr<Interaction> clone() const override;
	void interact(shared_ptr<Entity> owner, shared_ptr<Entity> interacter);
private:
	static const int OPEN_CHAR = '\'', CLOSED_CHAR = '+';
	bool open;
};