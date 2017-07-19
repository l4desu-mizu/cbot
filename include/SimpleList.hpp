#pragma once
#include "Entities.h"
#include <list>
#include <algorithm>
#include <mutex>

template <class X>
class SimpleList{
	public:
		SimpleList();
		SimpleList(X& entity);
		SimpleList(std::list<X>& entities);
		~SimpleList();
		bool contains(const X& entity);
		void add(X entity);
		void remove(const int id);
		void remove(const X& entity);
	private:
		std::mutex listMutex;
		std::list<X> entities;
};

template <class X>
inline
SimpleList<X>::SimpleList(){}

template <class X>
inline
SimpleList<X>::SimpleList(X& entity){
	this->add(entity);
}

template <class X>
inline
SimpleList<X>::SimpleList(std::list<X>& entities){
	this->entities=entities;
}

template <class X>
inline
SimpleList<X>::~SimpleList(){
	entities.clear();
}

template <class X>
inline
bool SimpleList<X>::contains(const X& entity){
	std::lock_guard<std::mutex> lock(listMutex);
	const auto it=std::find(entities.begin(), entities.end(), entity);
	return ( it != entities.end());
}

template <class X>
inline
void SimpleList<X>::add(X entity){
	std::lock_guard<std::mutex> lock(listMutex);
	entities.push_back(entity);
}

template <>
inline
void SimpleList<Entity>::remove(const int id){
	std::lock_guard<std::mutex> lock(listMutex);
	for(auto it=entities.begin();it!=entities.end();it++){
		if(it->getID()==id){
			entities.erase(it);
		}
	}
}

template <class X>
inline
void SimpleList<X>::remove(const int id){
	std::lock_guard<std::mutex> lock(listMutex);
	auto it=entities.begin();
	for(int i=0;i<id&&it!=entities.end();i++,it++){}
	if(it!=entities.end()){
		entities.erase(it);
	}
}

template <class X>
inline
void SimpleList<X>::remove(const X& entity){
	std::lock_guard<std::mutex> lock(listMutex);
	auto it=std::find(entities.begin(), entities.end(), entity);
	if( it != entities.end()) {
		entities.erase(it);
	}
}
