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
	if(text.to==*me||text.to==*currentChannel){
		respond(text);
	}
}
void Bot::notify(const Entity& e){
	if(e.getType()==EntityType::Channel_type){
		Channel c=e;
		channels.add(c);
		if(c.imHere()){
			currentChannel=&c;
		}
	}else if(e.getType()==EntityType::User_type){
		User u=e;
		users.add(u);
		if(u.isMe()){
			me=&u;
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
void Bot::run(){
	std::this_thread::sleep_for(std::chrono::seconds(1));
	if(me!=NULL){
		std::cout << "Ich heisse " << me->getName();
	}
	if(currentChannel!=NULL){
		std::cout << " und bin im Channel " << currentChannel->getName();
	}
	std::cout << std::endl;
}
