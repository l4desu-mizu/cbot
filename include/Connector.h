#pragma once
#include "EntityListener.h"
#include "TextListener.h"
#include "ConnectionListener.h"
#include <string>
#include <vector>
#include <mutex>

class Connector{
	public:
		virtual ~Connector(){}
		virtual void connect()=0;
		virtual void updateUserInfo(const User& u)=0;
		virtual void moveToTextChat(const Entity& c)=0;
		virtual void sendTextMessage(const std::string& message)=0;
		virtual void whisperTextMessage(const std::vector<User>& users, const std::string& message)=0;
		virtual void whisperTextMessage(const std::vector<Channel>& channels, const std::string& message)=0;
		//TODO virtual void disconnect()=0;?

		void addConnectionListener(ConnectionListener* l);
		void addChannelListener(EntityListener* l);
		void addUserListener(EntityListener* l);
		void addTextListener(TextListener* l);
	protected:
		void notifyListeners(const ConnectionEvent& event);
		void notifyListeners(const Text& text);
		void notifyListeners(const Entity& ent);
		void unnotifyListeners(const int id, const EntityType type);
	private:
		std::mutex connectionListenerMutex;
		std::vector<ConnectionListener*> connectionListeners;
		std::mutex channelListenerMutex;
		std::vector<EntityListener*> channelListeners;
		std::mutex userListenerMutex;
		std::vector<EntityListener*> userListeners;
		std::mutex textListenerMutex;
		std::vector<TextListener*> textListeners;
};
