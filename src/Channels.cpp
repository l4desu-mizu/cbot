#include "Channels.h"
#include <algorithm>

bool operator==(const Channel& lValue, const Channel& rValue){
	return lValue.id==rValue.id;
}

ChannelList::ChannelList(){}
ChannelList::ChannelList(Channel& channel){
	this->add(channel);
}
ChannelList::ChannelList(std::vector<Channel>& channels){
	this->channels=channels;
}
ChannelList::~ChannelList(){
	channels.clear();
}

bool ChannelList::contains(const Channel& channel){
	const auto it=std::find(channels.begin(), channels.end(), channel);
	return ( it != channels.end());
}

void ChannelList::add(Channel channel){
	channels.push_back(channel);
}

void ChannelList::remove(const int id){
	for(auto it=channels.begin();it!=channels.end();it++){
		if(it->id==id){
			channels.erase(it);
		}
	}
}

void ChannelList::remove(const Channel& channel){
	auto it=std::find(channels.begin(), channels.end(), channel);
	if( it != channels.end()) {
		channels.erase(it);
	}
}
