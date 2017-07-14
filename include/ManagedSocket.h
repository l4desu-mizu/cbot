#pragma once
#include "NameLookup.h"
#include <string>
#include <openssl/ssl.h>

class ManagedSocket{
	public:
		ManagedSocket(std::string hostname, int port);
		~ManagedSocket();
		void disconnect();
		int send(std::string message);
		int send(const char* message,int length);
		std::string receive();
	private:
		int socketfd;
		const int port;
		NameLookup::NameInfo* targetAddress=NULL;
};
