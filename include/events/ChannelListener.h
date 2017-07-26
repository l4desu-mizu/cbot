#pragma once

#include "model/Channel.h"
#include "events/EntityEvents.h"

class ChannelListener{
	public:
		virtual ~ChannelListener(){}
		virtual void notify(const Channel& e,const EntityEvent nEvent)=0;
};
