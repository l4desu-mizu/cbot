#pragma once
#include <string>

enum EntityType{
	Channel_type,
	User_type,
	None
};
class Entity{
	public:
		Entity();
		//Entity(const Entity& entity);
		~Entity();
		bool operator==(const Entity& otherEntity) const;
		Entity& operator=(const Entity& entity);
		void setID(const int newID);
		void setName(const std::string newName);
		void setConcern(const bool concern);
		int getID() const;
		std::string getName() const;
		EntityType getType() const;
		bool getConcern() const;
	protected:
		Entity(const int id,const std::string name, const EntityType type, bool myConcern=false);
		int id;
		std::string name;
		bool myConcern;
	private:
		const EntityType type;
};

class User:public Entity{
	public:
		User();
		User(const int id, const std::string name, bool me=false);
		User(const Entity& entity);
		~User();
		User& operator=(const Entity& entity);
		bool isMe();
};

class Channel:public Entity{
	public:
		Channel();
		Channel(const int id, const std::string name, bool here=false);
		Channel(const Entity& entity);
		~Channel();
		Channel& operator=(const Entity& entity);
		bool imHere();
		//TODO list of users?
};
