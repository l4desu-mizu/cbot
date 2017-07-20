#pragma once

#include "extensions/TextExtension.h"
#include <curl/curl.h>

class HttpRelay:public TextExtension{
	public:
		HttpRelay(const std::string url);
		~HttpRelay();
		Text relayMessage(const Text& text);
	private:
		CURL* curl=NULL;
};
