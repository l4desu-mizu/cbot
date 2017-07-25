#pragma once

#include "Channel.h"
#include "EntityEvents.h"

class ChannelListener{
	public:
		virtual ~ChannelListener(){}
		virtual void notify(const Channel& e,const EntityEvent nEvent)=0;
};
