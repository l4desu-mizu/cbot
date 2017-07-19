#pragma once

#include "Bot.h"

class SimpleBot:public Bot{
	public:
		SimpleBot(Connector* connection);
		~SimpleBot();
		void preRun();
	protected:
		bool respond(const Text& text);
};
