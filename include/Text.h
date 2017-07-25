#pragma once
#include "Entity.h"
#include <string>

struct Text{
	Text(std::string message,Entity from,Entity to,bool isPrivate=false):message(message),from(from),to(to),isPrivate(isPrivate){}
	std::string message;
	Entity from;
	Entity to;
	bool isPrivate;
};
