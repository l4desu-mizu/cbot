#pragma once

#include "Text.h"

class TextListener{
	public:
		virtual ~TextListener(){}
		virtual void notify(const Text& text)=0;
};
