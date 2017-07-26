#include "model/Channel.h"

Channel::Channel(const int id, const std::string name):
Entity(id,name){
}

Channel::~Channel(){}

bool Channel::operator==(const Channel& channel) const{
	return this->getID()==channel.getID();
}
