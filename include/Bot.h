#pragma once
#include "SimpleList.hpp"
#include "EntityListener.h"
#include "Connector.h"
#include "IBot.h"
#include <string>

class Bot: public EntityListener, IBot{
	public:
		Bot(Connector* connection);
		~Bot();
		void notify(const Entity& e);
		void unnotify(const int id);
		void run();
	private:
		Connector* connection;
		SimpleList<Channel> channels;
		SimpleList<User> users;
		std::string myName;
};
