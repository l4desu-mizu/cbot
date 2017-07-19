#pragma once
#include "EntityListener.h"
#include "TextListener.h"
#include <string>

class Connector{
	public:
		virtual ~Connector(){}
		virtual void sendTextMessage(const std::string& message)=0;
		virtual void connect()=0;
		virtual void addChannelListener(EntityListener* l)=0;
		virtual void addUserListener(EntityListener* l)=0;
		//TODO virtual void disconnect()=0;?
};
