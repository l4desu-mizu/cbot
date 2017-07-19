#pragma once

#include "Entities.h"

class EntityListener{
	public:
		virtual ~EntityListener();
		virtual void notify(const Entity& c);
		virtual void unnotify(const int id);
};
