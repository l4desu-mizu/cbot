#pragma once

#include "extensions/TextExtension.h"
#include <curl/curl.h>

class HttpRelayBot:public TextExtension{
	public:
		HttpRelayBot();
		~HttpRelayBot();
		Text relayMessage(const Text& text);
	private:
		CURL* curl=NULL;
};
