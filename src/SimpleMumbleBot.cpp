#include "SimpleMumbleBot.h"
#include <iostream>
#include <ctime>

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
		if(it->getName()==channel){
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
	if(text.message.find("time")!=std::string::npos){
		std::string response=getTime();
		connection->sendTextMessage(response);
	}
	return true;
}

std::string SimpleMumbleBot::getTime(){
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
	std::string str(buffer);
	return str;
}
