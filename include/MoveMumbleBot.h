#pragma once
#include "Bot.h"
#include <mutex>
#include <queue>

class MoveMumbleBot:public Bot{
	public:
		MoveMumbleBot(Connector* connection,const std::string targetChannel);
		~MoveMumbleBot();
		void notify(const Entity& e);
		bool run();
		bool preRun();
	protected:
		bool respond(const Text& text);
	private:
		const std::string targetChannelName;
		Channel targetChannel;
		std::mutex moveTargetsLock;
		std::queue<User> moveTargets;
};
