#pragma once
#include "model/Entity.h"

class Channel:public Entity{
	public:
		Channel();
		bool operator==(const Channel& channel) const;
		Channel(const int id, const std::string name);
		~Channel();
};
