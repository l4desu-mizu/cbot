#pragma once

#include "bot/Bot.h"
#include "extensions/http/HttpRelay.h"

#define HTTP_MUMBLE_BOT "HttpMumbleBot"

class HttpMumbleBot:public Bot{
	public:
		HttpMumbleBot(Connector* connection, std::string url, std::string prefferedChannel="");
		~HttpMumbleBot();
		bool preRun();
	protected:
		bool respond(const Text& text);
	private:
		const std::string prefferedChannel;
		HttpRelay* relay=NULL;
};

