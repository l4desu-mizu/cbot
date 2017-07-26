#include "MoveMumbleBot.h"


MoveMumbleBot::MoveMumbleBot(Connector* connection,const std::string targetChannel,const std::string defaultMessage):
Bot(connection),
targetChannelName(targetChannel),
defaultMessage(defaultMessage){
}
MoveMumbleBot::~MoveMumbleBot(){
	if(targetChannel!=NULL){
		delete targetChannel;
		targetChannel=NULL;
	}
}
bool MoveMumbleBot::preRun(){
	bool res=Bot::preRun();
	std::vector<Channel> chan=channels.getCurrent();
	if(targetChannelName.size()>0){
		for(auto it=chan.begin();it!=chan.end();it++){
			if(it->getName()==targetChannelName){
				if(targetChannel==NULL){
					targetChannel=new Channel(it->getID(),it->getName());
				}
				break;
			}
		}
	}
}
void MoveMumbleBot::notify(const User& e,const EntityEvent event){
	Bot::notify(e,event);
	if(event==EntityEvent::UpdateUser||event==EntityEvent::Add){
		User copy=e;
		Bot::updateUserData(&copy);
		std::lock_guard<std::mutex> lock(meLock);
		if(me!=NULL&&copy.getChannelID()==me->getChannelID()&&copy.getID()!=me->getID()){
			std::lock_guard<std::mutex> lock(moveTargetsLock);
			moveTargets.push(users.getCopy(copy));
		}
	}
}
bool MoveMumbleBot::run(){
	bool res=Bot::run();
	if(moveTargets.size()>0){
		std::lock_guard<std::mutex> lock(moveTargetsLock);
		User u=moveTargets.front();
		connection->whisperTextMessage(u,defaultMessage);
		connection->moveToTextChat(u,*targetChannel);
		moveTargets.pop();
	}
	return res;
}
bool MoveMumbleBot::respond(const Text& text){
	return true;
}
