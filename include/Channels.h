#pragma once
#include <string>
#include <vector>

struct Channel{
	int id;
	std::string name;
	friend bool operator==(const Channel& lValue, const Channel& rValue);
};

class ChannelList{
	public:
		ChannelList();
		ChannelList(Channel& channel);
		ChannelList(std::vector<Channel>& channels);
		~ChannelList();
		bool contains(const Channel& channel);
		void add(Channel channel);
		void remove(const int id);
		void remove(const Channel& channel);
	private:
		std::vector<Channel> channels;
};
