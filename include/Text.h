#pragma once
#include "Entities.h"
#include <string>

struct Text{
	std::string message;
	const Entity from;
	const Entity to;
};
