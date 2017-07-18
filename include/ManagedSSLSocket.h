#pragma once
#include <openssl/ssl.h>
#include <string>

class ManagedSSLSocket{
	public:
		ManagedSSLSocket(std::string hostname, int port, bool useEncrypt=true);
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
		void initSSLSocket(const std::string& hostname,const int& port);
		std::string createTargetHost(const std::string& host,const int& port);
};
