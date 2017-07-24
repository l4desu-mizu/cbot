#pragma once
#include "SimpleList.hpp"
#include "TextListener.h"
#include "EntityListener.h"
#include "ConnectionListener.h"
#include "Connector.h"
#include "IBot.h"
#include <string>
#include <mutex>
#include <queue>
#include <atomic>

class Bot: public ConnectionListener,public TextListener,public EntityListener,public IBot{
	public:
		Bot(Connector* connection);
		~Bot();
		void notify(const ConnectionEvent e);
		void notify(const Text& text);
		void notify(const Entity& e);
		void unnotify(const Entity& e);
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
		std::mutex channelLock;
		User me;
		Channel currentChannel;
		bool reconnect();
		virtual bool respond(const Text& text)=0;
		void updateData(Entity* ent);
	private:
		void clearQueue();
};
