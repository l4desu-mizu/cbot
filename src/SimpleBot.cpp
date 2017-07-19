#include "SimpleBot.h"

SimpleBot::SimpleBot(Connector* connection):Bot(connection){
}
SimpleBot::~SimpleBot(){
}
bool SimpleBot::respond(const Text& text){
	return true;
}
