#pragma once
#include <string>
#include <vector>

enum EntityType{
	Channel,
	User
};
class Entity{
	public:
		int id;
		std::string name;
		EntityType type;
		bool operator==(const Entity& otherEntity);
};

class EntityList{
	public:
		EntityList();
		EntityList(Entity& entity);
		EntityList(std::vector<Entity>& entitys);
		~EntityList();
		bool contains(const Entity& entity);
		void add(Entity entity);
		void remove(const int id);
		void remove(const Entity& entity);
	private:
		std::vector<Entity> entitys;
};
