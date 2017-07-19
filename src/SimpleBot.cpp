#include "SimpleBot.h"
#include <iostream>

SimpleBot::SimpleBot(Connector* connection):Bot(connection){
}
SimpleBot::~SimpleBot(){
}
void SimpleBot::preRun(){
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
bool SimpleBot::respond(const Text& text){
	return true;
}
