#include "Entities.h"
#include <algorithm>

Entity::Entity(int id,std::string name, EntityType type):id(id),name(name),type(type){
}
//Entity::Entity(const Entity& entity):id(entity.getID()),name(entity.getName()),type(entity.getType()){
//}
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

User::User(const int id, const std::string name):Entity(id,name,EntityType::User_type){
}
User::~User(){}
User& User::operator=(const User& otherEntity){
	this->id=otherEntity.getID();
	this->name=otherEntity.getName();
	return *this;
}
Channel::Channel(const int id, const std::string name):Entity(id,name,EntityType::Channel_type){
}
Channel::~Channel(){}
Channel& Channel::operator=(const Channel& otherEntity){
	this->id=otherEntity.getID();
	this->name=otherEntity.getName();
	return *this;
}

