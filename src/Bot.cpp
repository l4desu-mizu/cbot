#include "Bot.h"

Bot::Bot(Connector* connection):connection(connection){
	connection->connect();
}
Bot::~Bot(){
}
void Bot::notify(const Entity& e){
}
void Bot::unnotify(const int id){
}
void Bot::run(){
}
