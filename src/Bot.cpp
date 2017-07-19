#include "Bot.h"
#include <thread>
#include <iostream>

Bot::Bot(Connector* connection):connection(connection){
	connection->addChannelListener(this);
	connection->addUserListener(this);
	connection->connect();
}
Bot::~Bot(){
}
void Bot::notify(const Text& text){
	std::lock_guard<std::mutex> lock(textingMutex);
	receivedTexts.push(text);
}
void Bot::notify(const Entity& e){
	if(e.getType()==EntityType::Channel_type){
		Channel c=e;
		channels.add(c);
		if(c.imHere()){
			std::lock_guard<std::mutex> lock(channelLock);
			currentChannel=channels.getAllocated(c);
		}
	}else if(e.getType()==EntityType::User_type){
		User u=e;
		users.add(u);
		if(u.isMe()){
			std::lock_guard<std::mutex> lock(meLock);
			me=users.getAllocated(u);
		}
	}
}
void Bot::unnotify(const Entity& e){
	if(e.getType()==EntityType::Channel_type){
		channels.remove(e.getID());
	}else if(e.getType()==EntityType::User_type){
		users.remove(e.getID());
	}
}
void Bot::preRun(){
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
void Bot::run(){
	std::this_thread::sleep_for(std::chrono::seconds(1));
	if(receivedTexts.size()>0){
		std::lock_guard<std::mutex> lock(textingMutex);
		if(respond(receivedTexts.front())){
			receivedTexts.pop();
		}
	}
	std::cout << "messages " << receivedTexts.size() << std::endl;
}
