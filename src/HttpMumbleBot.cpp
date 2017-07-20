#include "HttpMumbleBot.h"
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
void HttpMumbleBot::preRun(){
	Bot::preRun();
	std::vector<Channel> chan=channels.getCurrent();
	for(auto it=chan.begin();it!=chan.end();it++){
		if(it->getName()==prefferedChannel){
			connection->moveToTextChat(*it);
			break;
		}
	}
}
bool HttpMumbleBot::respond(const Text& text){
	Text response=relay->relayMessage(text);
	if(response.isPrivate){
		std::vector<User> users;
		users.push_back(text.from);
		connection->whisperTextMessage(users,response.message);
	}else{
		connection->sendTextMessage(response.message);
	}
	return true;
}
