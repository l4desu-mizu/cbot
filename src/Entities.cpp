#include "Entities.h"
#include <algorithm>

bool operator==(const Entity& lValue, const Entity& rValue){
	return lValue.id==rValue.id&&lValue.type==rValue.type;
}

EntityList::EntityList(){}
EntityList::EntityList(Entity& entity){
	this->add(entity);
}
EntityList::EntityList(std::vector<Entity>& entitys){
	this->entitys=entitys;
}
EntityList::~EntityList(){
	entitys.clear();
}

bool EntityList::contains(const Entity& entity){
	const auto it=std::find(entitys.begin(), entitys.end(), entity);
	return ( it != entitys.end());
}

void EntityList::add(Entity entity){
	entitys.push_back(entity);
}

void EntityList::remove(const int id){
	for(auto it=entitys.begin();it!=entitys.end();it++){
		if(it->id==id){
			entitys.erase(it);
		}
	}
}

void EntityList::remove(const Entity& entity){
	auto it=std::find(entitys.begin(), entitys.end(), entity);
	if( it != entitys.end()) {
		entitys.erase(it);
	}
}
