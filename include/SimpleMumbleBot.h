#pragma once

#include "Bot.h"

class SimpleMumbleBot:public Bot{
	public:
		SimpleMumbleBot(Connector* connection);
		~SimpleMumbleBot();
		void preRun();
	protected:
		bool respond(const Text& text);
	private:
		std::string getTime();
};
