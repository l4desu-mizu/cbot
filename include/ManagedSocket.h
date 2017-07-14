#pragma once
#include "NameLookup.h"
#include <string>
#include <openssl/ssl.h>

class ManagedSocket{
	public:
		ManagedSocket(std::string hostname, int port);
		ManagedSocket(std::string hostname, int port, std::string certFile, std::string certKeyFile);
		~ManagedSocket();
		void disconnect();
		int send(std::string message);
		int send(const char* message,int length);
		std::string receive();
	private:
		int socketfd;
		const int port;
		const std::string certFile;
		const std::string certKeyFile;
		SSL* ssl=NULL;
		SSL_CTX* sslContext=NULL;
		NameLookup::NameInfo* targetAddress=NULL;
};
