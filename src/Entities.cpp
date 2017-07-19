#include "Entities.h"
#include <algorithm>

Entity::Entity(int id,std::string name, EntityType type, const bool myConcern):id(id),name(name),type(type),myConcern(myConcern){
}
bool Entity::operator==(const Entity& otherEntity){
	return this->id==otherEntity.getID()&&this->type==otherEntity.getType();
}
int Entity::getID() const{
	return id;
}
std::string Entity::getName() const{
	return name;
}
EntityType Entity::getType() const{
	return type;
}

User::User(const int id, const std::string name, const bool me):Entity(id,name,EntityType::User_type,me){
}
User::~User(){}
User& User::operator=(const User& otherEntity){
	this->id=otherEntity.getID();
	this->name=otherEntity.getName();
	return *this;
}
bool User::isMe(){
	return this->myConcern;
}
Channel::Channel(const int id, const std::string name, const bool here):Entity(id,name,EntityType::Channel_type,here){
}
Channel::~Channel(){}
Channel& Channel::operator=(const Channel& otherEntity){
	this->id=otherEntity.getID();
	this->name=otherEntity.getName();
	return *this;
}
bool Channel::imHere(){
	return this->myConcern;
}
