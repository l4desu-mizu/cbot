#pragma once

#include "Entities.h"

class EntityListener{
	public:
		virtual ~EntityListener(){}
		virtual void notify(const Entity e)=0;
		virtual void unnotify(const Entity e)=0;
};
