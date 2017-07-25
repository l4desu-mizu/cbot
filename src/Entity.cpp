#include "Entity.h"

Entity::Entity(const int id,const std::string name):
id(id),
name(name){
}

Entity::~Entity(){}

bool Entity::operator==(const Entity& entity) const{
	return this->getID()==entity.getID() &&
		this->getName()==entity.getName();
}

void Entity::setID(const int newID){
	this->id=newID;
}

void Entity::setName(const std::string newName){
	this->name=newName;
}

int Entity::getID() const{
	return id;
}

std::string Entity::getName() const{
	return name;
}
