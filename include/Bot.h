#pragma once
#include "SimpleList.hpp"
#include "TextListener.h"
#include "UserListener.h"
#include "ChannelListener.h"
#include "ConnectionListener.h"
#include "Connector.h"
#include "IBot.h"
#include <string>
#include <mutex>
#include <queue>
#include <atomic>

class Bot: public ConnectionListener,public TextListener,public UserListener,public ChannelListener,public IBot{
	public:
		Bot(Connector* connection);
		~Bot();
		void notify(const ConnectionEvent e);
		void notify(const Text& text);
		void notify(const User& e,const EntityEvent event);
		void notify(const Channel& e,const EntityEvent event);
		bool preRun();
		bool run();
	protected:
		Connector* connection;
		std::atomic<bool> connected;
		std::atomic<bool> die;
		SimpleList<Channel> channels;
		SimpleList<User> users;
		std::mutex textingMutex;
		std::queue<Text> receivedTexts;
		int lastTextQueueSize=0;
		std::string myName;
		std::mutex meLock;
		User* me=NULL;
		bool reconnect();
		virtual bool respond(const Text& text)=0;
		void updateUserData(User* user);
		void updateChannelData(Channel* user);
	private:
		void clearQueue();
};
