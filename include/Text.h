#pragma once
#include "Entities.h"
#include <string>

struct Text{
	std::string message;
	Entity from;
	Entity to;
	bool isPrivate=false;
};
