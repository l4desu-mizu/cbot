#include "Entities.h"
#include <algorithm>

Entity::Entity(const int id,const std::string name,const  EntityType type, const bool myConcern):id(id),name(name),type(type),myConcern(myConcern){
}
bool Entity::operator==(const Entity& otherEntity) const{
	return this->id==otherEntity.getID()&&this->type==otherEntity.getType();
}
void Entity::setID(const int newID){
	this->id=newID;
}
void Entity::setName(const std::string newName){
	this->name=newName;
}
void Entity::setConcern(const bool concern){
	this->myConcern=concern;
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
bool Entity::getConcern() const{
	return myConcern;
}

User::User(const int id, const std::string name, bool me):Entity(id,name,EntityType::User_type,me){
}
User::User(const Entity& entity):User(entity.getID(),entity.getName(),entity.getConcern()){
}
User::~User(){}
User& User::operator=(const Entity& otherEntity){
	this->id=otherEntity.getID();
	this->name=otherEntity.getName();
	return *this;
}
bool User::isMe(){
	return this->myConcern;
}
Channel::Channel(const int id, const std::string name, bool here):Entity(id,name,EntityType::Channel_type,here){
}
Channel::Channel(const Entity& entity):Channel(entity.getID(),entity.getName(),entity.getConcern()){
}
Channel::~Channel(){}
Channel& Channel::operator=(const Entity& otherEntity){
	this->id=otherEntity.getID();
	this->name=otherEntity.getName();
	return *this;
}
bool Channel::imHere(){
	return this->myConcern;
}
