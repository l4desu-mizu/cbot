#pragma once

#include "Entities.h"

class EntityListener{
	public:
		virtual ~EntityListener(){}
		virtual void notify(const Entity& c)=0;
		virtual void unnotify(const int id)=0;
};
