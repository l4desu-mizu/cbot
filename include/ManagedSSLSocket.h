#pragma once
#include "NameLookup.h"
#include <openssl/ssl.h>
#include <string>

class ManagedSSLSocket{
	public:
		ManagedSSLSocket(std::string hostname, int port);
		ManagedSSLSocket(std::string hostname, int port, std::string certFile, std::string certKeyFile);
		~ManagedSSLSocket();
		void disconnect();
		int send(std::string message);
		int send(const char* message,int length);
		std::string receive();
	private:
		const std::string certFile;
		const std::string certKeyFile;
		BIO* bio;
		SSL* ssl=NULL;
		SSL_CTX* sslContext=NULL;
		void initSSL();
		const char* createTargetHost(std::string host, int port);
};
