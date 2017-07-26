#pragma once
#include "bot/Bot.h"
#include <mutex>
#include <queue>

class MoveMumbleBot:public Bot{
	public:
		MoveMumbleBot(Connector* connection,const std::string targetChannel,const std::string defaultMessage);
		~MoveMumbleBot();
		void notify(const User& e,const EntityEvent event);
		bool run();
		bool preRun();
	protected:
		bool respond(const Text& text);
	private:
		const std::string targetChannelName;
		const std::string defaultMessage;
		Channel* targetChannel=NULL;
		std::mutex moveTargetsLock;
		std::queue<User> moveTargets;
};
