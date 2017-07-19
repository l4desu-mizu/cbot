#pragma once
#include <string>

enum EntityType{
	Channel_type,
	User_type
};
class Entity{
	public:
		//Entity(const Entity& entity);
		virtual ~Entity(){}
		bool operator==(const Entity& otherEntity);
		int getID() const;
		std::string getName() const;
		EntityType getType() const;
	protected:
		Entity(const int id,const std::string name, const EntityType type);
		int id;
		std::string name;
	private:
		const EntityType type;
};

class User:public Entity{
	public:
		User(const int id, const std::string name);
		~User();
		User& operator=(const User& entity);
};

class Channel:public Entity{
	public:
		Channel(const int id, const std::string name);
		~Channel();
		Channel& operator=(const Channel& entity);
};
