#pragma once

#include "bot/Bot.h"

class SimpleMumbleBot:public Bot{
	public:
		SimpleMumbleBot(Connector* connection, const std::string channel="");
		~SimpleMumbleBot();
		bool preRun();
	protected:
		bool respond(const Text& text);
	private:
		const std::string prefferedChannel;
		std::string getTime();
};
