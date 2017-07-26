#pragma once
#include <string>

class Entity{
	public:
		Entity(const int id,const std::string name);
		~Entity();
		bool operator==(const Entity& entity) const;
		void setID(const int newID);
		void setName(const std::string newName);
		int getID() const;
		std::string getName() const;
	private:
		int id;
		std::string name;
};

