#pragma once

#include "User.h"
#include "EntityEvents.h"

class UserListener{
	public:
		virtual ~UserListener(){}
		virtual void notify(const User& e,const EntityEvent nEvent)=0;
};
