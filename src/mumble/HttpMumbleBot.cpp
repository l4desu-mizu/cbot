#include "mumble/HttpMumbleBot.h"
#include <vector>

HttpMumbleBot::HttpMumbleBot(Connector* connection, std::string url, std::string prefferedChannel):
Bot(connection),
prefferedChannel(prefferedChannel){
	if(url.size()>0){
		relay=new HttpRelay(url);
	}else{
		throw std::runtime_error("No url provided!");
	}
}
HttpMumbleBot::~HttpMumbleBot(){
	if(relay!=NULL){
		delete relay;
		relay=NULL;
	}
}
bool HttpMumbleBot::preRun(){
	bool res=Bot::preRun();
	if(prefferedChannel.size()>0){
		std::vector<Channel> chan=channels.getCurrent();
		for(auto it=chan.begin();it!=chan.end();it++){
			if(it->getName()==prefferedChannel){
				connection->moveToTextChat(*it);
				break;
			}
		}
	}
	return res;
}
bool HttpMumbleBot::respond(const Text& text){
	Text response=relay->relayMessage(text);
	if(response.isPrivate){
		User user(text.from.getID(),text.from.getName());
		connection->whisperTextMessage(user,response.message);
	}else{
		connection->sendTextMessage(response.message);
	}
	return true;
}
