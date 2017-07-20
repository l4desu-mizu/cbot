#pragma once
#include "EntityListener.h"
#include "TextListener.h"
#include "ConnectionListener.h"
#include <string>
#include <vector>

class Connector{
	public:
		virtual ~Connector(){}
		virtual void connect()=0;
		virtual void updateUserInfo(const User& u)=0;
		virtual void moveToTextChat(const Entity& c)=0;
		virtual void sendTextMessage(const std::string& message)=0;
		virtual void whisperTextMessage(const std::vector<User>& users, const std::string& message)=0;
		virtual void whisperTextMessage(const std::vector<Channel>& channels, const std::string& message)=0;
		virtual void addConnectionListener(ConnectionListener* l)=0;
		virtual void addChannelListener(EntityListener* l)=0;
		virtual void addUserListener(EntityListener* l)=0;
		virtual void addTextListener(TextListener* l)=0;
		//TODO virtual void disconnect()=0;?
};
