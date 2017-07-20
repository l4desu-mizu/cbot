#pragma once
#include "SimpleList.hpp"
#include "TextListener.h"
#include "EntityListener.h"
#include "Connector.h"
#include "IBot.h"
#include <string>
#include <mutex>
#include <queue>

class Bot: public TextListener,EntityListener, IBot{
	public:
		Bot(Connector* connection);
		~Bot();
		void notify(const Text& text);
		void notify(const Entity& e);
		void unnotify(const Entity& e);
		void preRun();
		void run();
	protected:
		Connector* connection;
		SimpleList<Channel> channels;
		SimpleList<User> users;
		std::mutex textingMutex;
		std::queue<Text> receivedTexts;
		int lastTextQueueSize=0;
		std::string myName;
		std::mutex meLock;
		std::mutex channelLock;
		User* me=NULL;
		Channel* currentChannel=NULL;
		virtual bool respond(const Text& text)=0;
		User getUserData(const int id);
		Channel getChannelData(const int id);
		void updateData(Entity* ent);
	private:
		void clearQueue();
};
