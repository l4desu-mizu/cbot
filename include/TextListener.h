#pragma once

#include "Entities.h"
#include "Text.h"
#include <string>

class TextListener{
	public:
		virtual ~TextListener(){}
		virtual void notify(const Text& text)=0;
};
