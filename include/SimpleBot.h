#pragma once

#include "Bot.h"

class SimpleBot:public Bot{
	public:
		SimpleBot(Connector* connection);
		~SimpleBot();
	protected:
		bool respond(const Text& text);
};
