#pragma once
#include "SimpleList.hpp"
#include "TextListener.h"
#include "EntityListener.h"
#include "Connector.h"
#include "IBot.h"
#include <string>

class Bot: public TextListener,EntityListener, IBot{
	public:
		Bot(Connector* connection);
		~Bot();
		void notify(const Text& text);
		void notify(const Entity& e);
		void unnotify(const Entity& e);
		void run();
	protected:
		Connector* connection;
		SimpleList<Channel> channels;
		SimpleList<User> users;
		std::string myName;
		User* me=NULL;
		Channel* currentChannel=NULL;
		virtual void respond(const Text& text)=0;
};
