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
		Entity(const int id,const std::string name, const EntityType type, bool myConcern);
		int id;
		std::string name;
		bool myConcern;
	private:
		const EntityType type;
};

class User:public Entity{
	public:
		User(const int id, const std::string name, bool me=false);
		~User();
		User& operator=(const User& entity);
		bool isMe();
};

class Channel:public Entity{
	public:
		Channel(const int id, const std::string name, bool here=false);
		~Channel();
		Channel& operator=(const Channel& entity);
		bool imHere();
		//TODO list of users
};
