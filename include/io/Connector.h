#pragma once
#include "events/ChannelListener.h"
#include "events/UserListener.h"
#include "events/TextListener.h"
#include "events/ConnectionListener.h"
#include <string>
#include <vector>
#include <mutex>

class Connector{
	public:
		virtual ~Connector(){}
		virtual void connect()=0;
		virtual void disconnect()=0;
		virtual void updateUserInfo(const User& u)=0;
		virtual void moveToTextChat(const Channel& c)=0;
		virtual void moveToTextChat(const User& u,const Channel& c)=0;
		virtual void sendTextMessage(const std::string& message)=0;
		virtual void whisperTextMessage(const User& user, const std::string& message)=0;
		virtual void whisperTextMessage(const Channel& channel, const std::string& message)=0;
		virtual void whisperTextMessage(const std::vector<User>& users, const std::string& message)=0;
		virtual void whisperTextMessage(const std::vector<Channel>& channels, const std::string& message)=0;

		void addConnectionListener(ConnectionListener* l);
		void addChannelListener(ChannelListener* l);
		void addUserListener(UserListener* l);
		void addTextListener(TextListener* l);
		void removeConnectionListener(ConnectionListener* l);
		void removeChannelListener(ChannelListener* l);
		void removeUserListener(UserListener* l);
		void removeTextListener(TextListener* l);
	protected:
		void notifyListeners(const ConnectionEvent& event);
		void notifyListeners(const Text& text);
		void notifyListeners(const Channel& ent,const EntityEvent event);
		void notifyListeners(const User& ent,const EntityEvent event);
	private:
		std::mutex connectionListenerMutex;
		std::vector<ConnectionListener*> connectionListeners;
		std::mutex channelListenerMutex;
		std::vector<ChannelListener*> channelListeners;
		std::mutex userListenerMutex;
		std::vector<UserListener*> userListeners;
		std::mutex textListenerMutex;
		std::vector<TextListener*> textListeners;
};
