#include "Bot.h"
#include <thread>

#define FLOOD_WARNING 8

Bot::Bot(Connector* connection):
connection(connection),
connected(false),
die(false){
	connection->addChannelListener(this);
	connection->addUserListener(this);
	connection->addTextListener(this);
	connection->addConnectionListener(this);
	connection->connect();
}
Bot::~Bot(){
	connection->disconnect();
	connection->removeChannelListener(this);
	connection->removeUserListener(this);
	connection->removeTextListener(this);
	connection->removeConnectionListener(this);
	std::lock_guard<std::mutex> lockMe(meLock);
	if(me!=NULL){
		delete me;
		me=NULL;
	}
}
void Bot::notify(const ConnectionEvent e){
	switch(e){
		case ConnectionEvent::Connect:
			connected=true;
			break;
		case ConnectionEvent::Ban:
			die=true;
		case ConnectionEvent::Disconnect:
		case ConnectionEvent::Kick: //always reset bot state
			connected=false;
			channels.clear();
			users.clear();
			if(me!=NULL){
				std::lock_guard<std::mutex> lockMe(meLock);
				delete me;
				me=NULL;
			}
			clearQueue();
	}
}
void Bot::notify(const Text& text){
	Text localText=text;
	updateUserData(reinterpret_cast<User*>(&localText.from));
	if(localText.isPrivate){
		updateUserData(reinterpret_cast<User*>(&localText.to));
	}else{
		updateChannelData(reinterpret_cast<Channel*>(&localText.to));
	}
	std::lock_guard<std::mutex> lock(textingMutex);
	receivedTexts.push(localText);
}
void Bot::notify(const Channel& e,const EntityEvent event){
	switch(event){
		case EntityEvent::Add:
			channels.add(e);
			break;
		case EntityEvent::Remove:
			channels.remove(e.getID());
			break;
	}
}
void Bot::notify(const User& e,const EntityEvent event){
	switch(event){
		case EntityEvent::UpdateUser:
			{
				std::lock_guard<std::mutex> lock(meLock);
				if(me==NULL){//unknown user
					const int chanID=(e.getChannelID()!=-1)?e.getChannelID():0;//default channel root=0
					me = new User(e.getID(),e.getName(),chanID);
				}else{//known user but state changed
					me->setID(e.getID());
					if(e.getName().size()>0){
						me->setName(e.getName());
					}
					if(e.getChannelID()!=-1){
						me->setChannelID(e.getChannelID());
					}
				}
			}
		case EntityEvent::Add://still add user to list
			if(!users.contains(e)&&e.getChannelID()==-1){//set users channel to default if unknown
				User newUser=e;
				newUser.setChannelID(0);
				users.add(newUser);
			}else{
				users.add(e);//updates
			}
			break;
		case EntityEvent::Remove:
			if(e.getID()==me->getID()){//known user but got deleted
				std::lock_guard<std::mutex> lock(meLock);
				delete me;
				me=NULL;
			}
			users.remove(e.getID());
			break;
	}
}

bool Bot::preRun(){
	return true;
}
bool Bot::run(){
	if(die){
		return false;
	}
	if(!connected){//skip if connected
		return reconnect();
	}
	if(lastTextQueueSize+FLOOD_WARNING<receivedTexts.size()){
		lastTextQueueSize=0;
		clearQueue();
		connection->sendTextMessage("stop Spaming!");
	}
	if((lastTextQueueSize=receivedTexts.size())>0){
		std::lock_guard<std::mutex> lock(textingMutex);
		if(respond(receivedTexts.front())){
			receivedTexts.pop();
		}
	}
	return true;
}

bool Bot::reconnect(){
	connection->disconnect();
	try{
		connection->connect();
	}catch(std::runtime_error& e){
		connection->disconnect();
		die=true;
		return false;
	}
	return preRun();//"reinit"
}

void Bot::updateUserData(User* user){
	try{
		User savedUser=users.getCopy(*user);//can throw
		user->setName(savedUser.getName());//wont occur if thrown
		user->setChannelID(savedUser.getChannelID());
	}catch(std::runtime_error& e){
		//didn't find user so cant update info
	}
}

void Bot::updateChannelData(Channel* channel){
	try{
		Channel savedChannel=channels.getCopy(*channel);
		channel->setName(savedChannel.getName());
	}catch(std::runtime_error& e){
		//didn't find channel so cant update info
	}
}

void Bot::clearQueue(){
	std::queue<Text> empty;
	std::lock_guard<std::mutex> lock(textingMutex);
	std::swap(receivedTexts,empty);
	lastTextQueueSize=0;
}

