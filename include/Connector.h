#pragma once
#include <string>

class Connector{
	public:
		virtual ~Connector(){}
		virtual void sendTextMessage(const std::string& message)=0;
		virtual void connect()=0;
		//TODO virtual void disconnect()=0;?
};
