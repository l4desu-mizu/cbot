#include "SimpleMumbleBot.h"
#include <iostream>

SimpleMumbleBot::SimpleMumbleBot(Connector* connection):Bot(connection){
}
SimpleMumbleBot::~SimpleMumbleBot(){
}
void SimpleMumbleBot::preRun(){
	Bot::preRun();
	const std::string channel="SomeChannel";
	std::cout << "moving to " << channel << std::endl;
	std::vector<Channel> chan=channels.getCurrent();
	for(auto it=chan.begin();it!=chan.end();it++){
		if(it->getName()==channel){//currently hardcoded, setup via configs?
			connection->moveToTextChat(*it);
			break;
		}
	}
	const std::string greetings="Greetings!";
	std::vector<User> use=users.getCurrent();
	const std::string friends="How are you??";
	connection->whisperTextMessage(chan,greetings);
	connection->whisperTextMessage(use,friends);
}
bool SimpleMumbleBot::respond(const Text& text){
	return true;
}
