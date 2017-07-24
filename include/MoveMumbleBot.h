#pragma once
#include "Bot.h"
#include <mutex>
#include <queue>

class MoveMumbleBot:public Bot{
	public:
		MoveMumbleBot(Connector* connection,const std::string targetChannel,const std::string defaultMessage);
		~MoveMumbleBot();
		void notify(const Entity& e);
		bool run();
		bool preRun();
	protected:
		bool respond(const Text& text);
	private:
		const std::string targetChannelName;
		const std::string defaultMessage;
		Channel targetChannel;
		std::mutex moveTargetsLock;
		std::queue<User> moveTargets;
};
