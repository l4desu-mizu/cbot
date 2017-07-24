#include "MoveMumbleBot.h"
#include <iostream>


MoveMumbleBot::MoveMumbleBot(Connector* connection,const std::string targetChannel,const std::string defaultMessage):
Bot(connection),
targetChannelName(targetChannel),
defaultMessage(defaultMessage){
}
MoveMumbleBot::~MoveMumbleBot(){
}
bool MoveMumbleBot::preRun(){
	bool res=Bot::preRun();
	std::vector<Channel> chan=channels.getCurrent();
	if(targetChannelName.size()>0){
		for(auto it=chan.begin();it!=chan.end();it++){
			if(it->getName()==targetChannelName){
				targetChannel=(*it);
				break;
			}
		}
	}
}
void MoveMumbleBot::notify(const Entity& e){
	std::cout << "movebot notify" << std::endl;
	Bot::notify(e);
	Entity copy=e;
	Bot::updateData(&copy);
	if(copy.getType()==EntityType::User_type&&e.getID()!=me.getID()){
		std::lock_guard<std::mutex> lock(moveTargetsLock);
		moveTargets.push(users.getCopy(copy));
	}
}
bool MoveMumbleBot::run(){
	bool res=Bot::run();
	if(moveTargets.size()>0){
		std::lock_guard<std::mutex> lock(moveTargetsLock);
		User u=moveTargets.front();
		connection->whisperTextMessage(u,defaultMessage);
		connection->moveToTextChat(u,targetChannel);
		moveTargets.pop();
	}
	return res;
}
bool MoveMumbleBot::respond(const Text& text){
	return true;
}
