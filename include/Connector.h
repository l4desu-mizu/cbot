#pragma once
#include <string>

class Connector{
	public:
		virtual ~Connector(){}
		virtual void sendTextMessage(std::string message)=0;
};
