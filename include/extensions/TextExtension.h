#pragma once
#include "model/Text.h"

class TextExtension{
	public:
		virtual ~TextExtension(){}
		virtual Text relayMessage(const Text& text)=0;
};
