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
}
void Bot::notify(const ConnectionEvent e){
	switch(e){
		case ConnectionEvent::Connect:
			connected=true;
			break;
		case ConnectionEvent::Ban:
		case ConnectionEvent::Disconnect:
			die=true;
		case ConnectionEvent::Kick: //always reset bot state
			connected=false;
			channels.clear();
			users.clear();
			std::lock_guard<std::mutex> lockMe(meLock);
			std::lock_guard<std::mutex> lockChannel(channelLock);
			me=NULL;
			currentChannel=NULL;
			clearQueue();
	}
}
void Bot::notify(const Text& text){
	Text localText=text;
	updateData(&localText.from);
	updateData(&localText.to);
	std::lock_guard<std::mutex> lock(textingMutex);
	receivedTexts.push(localText);
}
void Bot::notify(const Entity& e){
	if(e.getType()==EntityType::Channel_type){
		Channel c=e;
		channels.add(c);
		if(c.imHere()){
			std::lock_guard<std::mutex> lock(channelLock);
			if(currentChannel!=NULL){
				currentChannel->setConcern(false);
			}
			currentChannel=channels.getAllocated(c);
		}
	}else if(e.getType()==EntityType::User_type){
		User u=e;
		users.add(u);
		if(u.isMe()){
			std::lock_guard<std::mutex> lock(meLock);
			if(me!=NULL){
				me->setConcern(false);
			}
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
User Bot::getUserData(const int id){
	SimpleList<Entity>* ents=reinterpret_cast<SimpleList<Entity>*>(&users);
	User user=*ents->getAllocated(id);
	return user;
}
Channel Bot::getChannelData(const int id){
	SimpleList<Entity>* ents=reinterpret_cast<SimpleList<Entity>*>(&channels);
	Channel channel=*ents->getAllocated(id);
	return channel;
}
void Bot::updateData(Entity* ent){
	Entity* pEnt=NULL;
	if(ent->getType()==EntityType::User_type){
		pEnt=users.getAllocated(*ent);
	}else if(ent->getType()==EntityType::Channel_type){
		pEnt=channels.getAllocated(*ent);
	}
	if(pEnt==NULL){
		return;
	}
	ent->setConcern(pEnt->getConcern());
	ent->setName(pEnt->getName());
}

void Bot::clearQueue(){
	std::queue<Text> empty;
	std::lock_guard<std::mutex> lock(textingMutex);
	std::swap(receivedTexts,empty);
	lastTextQueueSize=0;
}

