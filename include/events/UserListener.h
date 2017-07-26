#pragma once

#include "model/User.h"
#include "events/EntityEvents.h"

class UserListener{
	public:
		virtual ~UserListener(){}
		virtual void notify(const User& e,const EntityEvent nEvent)=0;
};
