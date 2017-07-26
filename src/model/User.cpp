#include "model/User.h"

User::User(const int id, const std::string name, const int channelID):
Entity(id,name),channelID(channelID){
}

User::~User(){}

bool User::operator==(const User& u) const{
	return this->getID()==u.getID();
}

int User::getChannelID() const{
	return channelID;
}

void User::setChannelID(int id){
	this->channelID=id;
}
