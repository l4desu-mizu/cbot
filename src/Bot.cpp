#include "Bot.h"

Bot::Bot(Connector* connection):connection(connection){
	connection->connect();
}
Bot::~Bot(){
}
void Bot::notify(const Entity& e){
	if(e.getType()==EntityType::Channel_type){
		Channel c=e;
		channels.add(c);
	}else if(e.getType()==EntityType::User_type){
		User u=e;
		users.add(u);
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
}
