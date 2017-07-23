#include "Connector.h"
#include <algorithm>

void Connector::addConnectionListener(ConnectionListener* l){
	std::lock_guard<std::mutex> lock(connectionListenerMutex);
	connectionListeners.push_back(l);
}

void Connector::addChannelListener(EntityListener* l){
	std::lock_guard<std::mutex> lock(channelListenerMutex);
	channelListeners.push_back(l);
}

void Connector::addUserListener(EntityListener* l){
	std::lock_guard<std::mutex> lock(userListenerMutex);
	userListeners.push_back(l);
}

void Connector::addTextListener(TextListener* l){
	std::lock_guard<std::mutex> lock(textListenerMutex);
	textListeners.push_back(l);
}

void Connector::removeConnectionListener(ConnectionListener* l){
	std::lock_guard<std::mutex> lock(connectionListenerMutex);
	auto it=std::find(connectionListeners.begin(),connectionListeners.end(),l);
	if(it!=connectionListeners.end()){
		connectionListeners.erase(it);
	}
}

void Connector::removeChannelListener(EntityListener* l){
	std::lock_guard<std::mutex> lock(channelListenerMutex);
	auto it=std::find(channelListeners.begin(),channelListeners.end(),l);
	if(it!=channelListeners.end()){
		channelListeners.erase(it);
	}
}

void Connector::removeUserListener(EntityListener* l){
	std::lock_guard<std::mutex> lock(userListenerMutex);
	auto it=std::find(userListeners.begin(),userListeners.end(),l);
	if(it!=userListeners.end()){
		userListeners.erase(it);
	}
}

void Connector::removeTextListener(TextListener* l){
	std::lock_guard<std::mutex> lock(textListenerMutex);
	auto it=std::find(textListeners.begin(),textListeners.end(),l);
	if(it!=textListeners.end()){
		textListeners.erase(it);
	}
}


void Connector::notifyListeners(const ConnectionEvent& event){
	std::lock_guard<std::mutex> lock(connectionListenerMutex);
	for(ConnectionListener* l:connectionListeners){
		l->notify(event);
	}
}

void Connector::notifyListeners(const Text& text){
	std::lock_guard<std::mutex> lock(textListenerMutex);
	for(TextListener* l:textListeners){
		l->notify(text);
	}
}
void Connector::notifyListeners(const Entity& ent){
	if(ent.getType()==EntityType::Channel_type){
		std::lock_guard<std::mutex> lock(channelListenerMutex);
		for(EntityListener* l:channelListeners){
			l->notify(ent);
		}
	}else if(ent.getType()==EntityType::User_type){
		std::lock_guard<std::mutex> lock(userListenerMutex);
		for(EntityListener* l:userListeners){
			l->notify(ent);
		}
	}
}
void Connector::unnotifyListeners(const int id, const EntityType type){
	if(type==EntityType::Channel_type){
		std::lock_guard<std::mutex> lock(channelListenerMutex);
		for(EntityListener* l:channelListeners){
			const Channel c(id,"");
			l->unnotify(c);
		}
	}else if(type==EntityType::User_type){
		std::lock_guard<std::mutex> lock(userListenerMutex);
		for(EntityListener* l:userListeners){
			const User u(id,"");
			l->unnotify(u);
		}
	}
}

