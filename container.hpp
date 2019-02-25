#pragma once

class Container {
public:
	int size; // Max container size
	vector<shared_ptr<Entity>> inventory;

	Container();
	Container(int size);
	~Container();

	bool canAdd(shared_ptr<Entity> entity);
	bool add(shared_ptr<Entity> entity);
	
	void del(shared_ptr<Entity> entity);
};