#pragma once

class Container {
public:
	int size; // Max container size
	std::vector<std::shared_ptr<Entity>> inventory;

	Container(int size);
	~Container();

	bool add(std::shared_ptr<Entity> entity);
	
	void del(std::shared_ptr<Entity> entity);
};