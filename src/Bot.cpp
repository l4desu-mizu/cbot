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
}
void Bot::notify(const ConnectionEvent e){
	switch(e){
		case ConnectionEvent::Connect:
			connected=true;
			break;
		case ConnectionEvent::Ban:
			die=true;
		case ConnectionEvent::Kick: //always do disconnect stuff
		case ConnectionEvent::Disconnect:
			connected=false;
			channels.clear();
			users.clear();
			me=NULL;
			currentChannel=NULL;
			clearQueue();
	}

	if(e==ConnectionEvent::Disconnect||e==ConnectionEvent::Kick){
	}else if(e==ConnectionEvent::Ban){
	}else if(e==ConnectionEvent::Connect){
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
}
void Bot::run(){
	if(die){
		throw std::runtime_error("They dun want meh QQ");
	}
	if(!connected){//skip if not connected
		return;
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
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
}

void Bot::clearQueue(){
	std::queue<Text> empty;
	std::swap(receivedTexts,empty);
	lastTextQueueSize=0;
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
