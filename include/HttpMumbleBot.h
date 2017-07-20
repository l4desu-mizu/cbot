#pragma once

#include "Bot.h"
#include "extensions/http/HttpRelay.h"

class HttpMumbleBot:public Bot{
	public:
		HttpMumbleBot(Connector* connection, std::string url, std::string prefferedChannel="");
		~HttpMumbleBot();
		void preRun();
	protected:
		bool respond(const Text& text);
	private:
		const std::string prefferedChannel;
		HttpRelay* relay=NULL;
};

