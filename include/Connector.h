#pragma once
#include <string>

class Connector{
	public:
		virtual ~Connector(){}
		virtual void sendTextMessage(const std::string& message)=0;
};
