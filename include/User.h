#pragma once
#include "Entity.h"

class User:public Entity{
	public:
		User(const int id, const std::string name="",const int channelID=-1);
		~User();
		bool operator==(const User& u) const;
		int getChannelID() const;
		void setChannelID(int id);
	private:
		int channelID;
};
